import { readFileSync } from 'fs'
import path from 'path'
import wasmTokenizer from './tokenizer'

type VectorUint8 = {
  size: () => number
  push_back: (byte: number) => void
  get: (index: number) => number
}

export const WASMTokenizer = async (base = 'cl100k_base.bin') => {
  const data = readFileSync(path.resolve(__dirname, base))
  const uint8Array = new Uint8Array(data)

  const TokenizerModule = await wasmTokenizer()

  // Create a vector from the Uint8Array
  const vector = new TokenizerModule.VectorUint8()
  for (let i = 0; i < uint8Array.length; i++) {
    vector.push_back(uint8Array[i])
  }
  const tokenizer = new TokenizerModule.Tokenizer(vector)
  vector.delete()
  return {
    encode: (text: string): VectorUint8 => tokenizer.encode(text),
    count: (text: string) => tokenizer.count(text),
    decode: (tokens: VectorUint8) => tokenizer.decode(tokens),
  }
}
