# wasm-tokenizer

wasm-tokenizer is a high-performance tokenizer written in C++ and compiled to WebAssembly (WASM) for use in both browser and Node.js environments. It provides efficient encoding and decoding of tokens, making it the most performant tokenizer in its class.

## Features

- Written in C++ and compiled to WebAssembly
- Compatible with browser and Node.js environments
- Highly efficient encoding and decoding of tokens
- Includes a tool to convert tiktoken file format to binary, reducing the size of cl100k token database by 60%
- Developed to enhance performance for token calculation on the frontend for [GPTunneL](https://gptunnel.com?utm_source=github&utm_campaign=wasm-tokenizer), an AI Aggregator by ScriptHeads

## Performance

wasm-tokenizer outperforms other popular tokenizers such as gpt-tokenizer and tiktoken. Here are some performance comparisons:

### 1,000 Tokens
![1k tokens performance](https://sh-bucket.storage.yandexcloud.net/blog/6699f7a8346a2a0001061dd9.webp)

### 10,000 Tokens
![10k tokens performance](https://sh-bucket.storage.yandexcloud.net/blog/6699f7d1346a2a0001061ddb.webp)

### 1,000,000 Tokens
![1m tokens performance](https://sh-bucket.storage.yandexcloud.net/blog/6699f7e0346a2a0001061ddc.webp)

All tests were run on a MacBook M1 Pro using Node.js v21.6.2, with 1000 iterations to calculate average time.

## Usage

### Node.js Environment

```typescript
import WASMTokenizer from './WASMTokenizer'

WASMTokenizer().then((tokenizer) => {
  const text = "Hello world";
  const length = tokenizer.count(text);
  const tokens = tokenizer.encode(text);
  const decoded = tokenizer.decode(tokens);

  console.log(decoded, length, 'tokens');
});
```

### Browser runtime

Using wasm-tokenizer in a web environment requires loading the WebAssembly module and the token database. Here's an example of how to use it:


```html
<head>
  <script src="tokenizer.js"></script>
</head>
<body>
  <script>
    fetch('cl100k_base.bin')
    .then(response => response.arrayBuffer())
    .then(arrayBuffer => {
      const uint8Array = new Uint8Array(arrayBuffer);
      wasmTokenizer().then((TokenizerModule) => {
        // Create a vector from the Uint8Array
        const vector = new TokenizerModule.VectorUint8();
        for (let i = 0; i < uint8Array.length; i++) {
          vector.push_back(uint8Array[i]);
        }

        // Create the tokenizer using the vector
        const tokenizer = new TokenizerModule.Tokenizer(vector);
        // Clean up the vector
        vector.delete();

        const text = "Hello world";
        const length = tokenizer.count(text);
        const tokens = tokenizer.encode(text);
        const decoded = tokenizer.decode(tokens);

        console.log(decoded, length, 'tokens');
      });
    });
  </script>
<body>
```


## Binary Conversion Tool

wasm-tokenizer includes a tool to convert tiktoken file format to binary. This conversion reduces the size of the cl100k token database by 60%, further improving performance and reducing resource usage.

## Real-world Application

You can see wasm-tokenizer in action on [GPTunneL](https://gptunnel.com?utm_source=github&utm_campaign=wasm-tokenizer), where it's used to power token calculations for various LLM models. Visit GPTunneL to experience the performance benefits of wasm-tokenizer and explore some of the most powerful language models available.

## Contributing

We welcome contributions to wasm-tokenizer! If you'd like to contribute, please follow these steps:

- Fork the repository
- Create a new branch for your feature or bug fix
- Make your changes and commit them with clear, descriptive messages
- Push your changes to your fork
- Submit a pull request to the main repository

## TODO

 - npm package
 - add all databases
 - add chat encode support

## License

This project is licensed under the MIT License.

## Acknowledgements

wasm-tokenizer was developed by ScriptHeads for use in the [GPTunneL](https://gptunnel.com?utm_source=github&utm_campaign=wasm-tokenizer) AI Aggregator. We thank the open-source community for their continuous support and contributions.