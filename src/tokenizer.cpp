#include <emscripten.h>
#include <emscripten/bind.h>
#include <vector>
#include <iostream>
#include <string>
#include <string_view>
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <memory>
#include <stdexcept>

class TrieNode {
public:
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    uint32_t token_id = 0;
};

class Tokenizer {
private:
    std::unique_ptr<TrieNode> root;
    std::vector<std::string> id_to_token;

    void insert(const std::string& token, uint32_t id) {
        TrieNode* node = root.get();
        for (char c : token) {
            if (!node->children[c]) {
                node->children[c] = std::make_unique<TrieNode>();
            }
            node = node->children[c].get();
        }
        node->token_id = id + 1;  // Add 1 to avoid 0 as a valid token ID
    }

public:
    Tokenizer(const std::vector<uint8_t>& data) : root(std::make_unique<TrieNode>()) {
        size_t i = 0;
        uint32_t id = 0;
        while (i < data.size()) {
            size_t len = data[i];
            i++;
            if (len == 0 || i + len > data.size()) {
                std::cout << "Invalid length at position " << i-1 << ": " << len << std::endl;
                break;
            }
            std::string token(data.begin() + i, data.begin() + i + len);
            insert(token, id);
            id_to_token.push_back(token);
            
            i += len;
            id++;
        }
    }

    std::vector<uint32_t> encode(const std::string& text) const {
        std::vector<uint32_t> result;
        result.reserve(text.length() / 2); 
        size_t i = 0;
        while (i < text.length()) {
            const TrieNode* node = root.get();
            size_t j = i;
            size_t last_match = i;
            uint32_t last_token_id = 0;

            while (j < text.length() && node->children.count(text[j])) {
                node = node->children.at(text[j]).get();
                j++;
                if (node->token_id != 0) {
                    last_match = j;
                    last_token_id = node->token_id;
                }
            }

            if (last_match > i) {
                result.push_back(last_token_id);
                i = last_match;
            } else {
                std::cout << "Unknown token at position " << i << ": '" << text[i] << "'" << std::endl;
                i++;
            }
        }
        return result;
    }

    std::string decode(const std::vector<uint32_t>& tokens) const {
        std::string result;
        for (uint32_t token : tokens) {
            if (token == 0 || token > id_to_token.size()) {
                std::cout << "Invalid token ID: " << token << std::endl;
                continue;
            }
            result += id_to_token[token - 1];  // Subtract 1 because we added 1 during encoding
        }
        return result;
    }

    size_t count(const std::string& text) const {
        return encode(text).size();
    }
};

// Helper function to convert JS array to std::vector
std::vector<uint8_t> vecFromJSArray(const emscripten::val& jsArray) {
    if (jsArray.typeOf().as<std::string>() != "object" || !jsArray["byteLength"].as<bool>()) {
        throw std::runtime_error("Invalid input: expected ArrayBuffer or TypedArray");
    }

    unsigned int length = jsArray["byteLength"].as<unsigned int>();
    std::vector<uint8_t> result(length);

    emscripten::val memoryView = emscripten::val::global("Uint8Array").new_(jsArray);
    emscripten::val memoryViewPtr = memoryView["byteOffset"];
    
    uint8_t* dataPtr = reinterpret_cast<uint8_t*>(memoryViewPtr.as<uintptr_t>());
    std::memcpy(result.data(), dataPtr, length);

    return result;
}

// Helper function to convert std::vector to JS array
emscripten::val vecToJSArray(const std::vector<uint32_t>& vec) {
    return emscripten::val::array(vec);
}

// Wrapper functions for exception handling
emscripten::val encode(Tokenizer* tokenizer, const std::string& text) {
    try {
        auto result = tokenizer->encode(text);
        return vecToJSArray(result);
    } catch (const std::exception& e) {
        std::cerr << "Error during tokenization: " << e.what() << std::endl;
        return emscripten::val::null();
    } catch (...) {
        std::cerr << "Unknown error during tokenization" << std::endl;
        return emscripten::val::null();
    }
}

emscripten::val decode(Tokenizer* tokenizer, const emscripten::val& jsArray) {
    try {
        std::vector<uint32_t> tokens;
        const auto l = jsArray["length"].as<unsigned>();
        tokens.reserve(l);
        for (unsigned i = 0; i < l; ++i) {
            tokens.push_back(jsArray[i].as<uint32_t>());
        }
        return emscripten::val(tokenizer->decode(tokens));
    } catch (const std::exception& e) {
        std::cerr << "Error during decoding: " << e.what() << std::endl;
        return emscripten::val::null();
    } catch (...) {
        std::cerr << "Unknown error during decoding" << std::endl;
        return emscripten::val::null();
    }
}

size_t count(Tokenizer* tokenizer, const std::string& text) {
    try {
        return tokenizer->count(text);
    } catch (const std::exception& e) {
        std::cerr << "Error during token counting: " << e.what() << std::endl;
        return 0;
    } catch (...) {
        std::cerr << "Unknown error during token counting" << std::endl;
        return 0;
    }
}

// Emscripten bindings
EMSCRIPTEN_BINDINGS(tokenizer_module) {
    emscripten::register_vector<uint8_t>("VectorUint8");
    emscripten::register_vector<uint32_t>("VectorUint32");

    emscripten::class_<Tokenizer>("Tokenizer")
        .constructor<const std::vector<uint8_t>&>()
        .function("encode", &Tokenizer::encode)
        .function("decode", &Tokenizer::decode)
        .function("count", &Tokenizer::count);

    emscripten::function("encode", &encode, emscripten::allow_raw_pointers());
    emscripten::function("decode", &decode, emscripten::allow_raw_pointers());
    emscripten::function("count", &count, emscripten::allow_raw_pointers());
}

// Optional: C-style function for creating Tokenizer from raw pointer
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    Tokenizer* createTokenizerFromPointer(uint8_t* ptr, size_t length) {
        try {
            std::vector<uint8_t> data(ptr, ptr + length);
            return new Tokenizer(data);
        } catch (const std::exception& e) {
            std::cerr << "Error creating Tokenizer: " << e.what() << std::endl;
            return nullptr;
        } catch (...) {
            std::cerr << "Unknown error creating Tokenizer" << std::endl;
            return nullptr;
        }
    }
}