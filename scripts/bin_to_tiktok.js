const fs = require('fs')

let count = 0
let length = 0
let tiktokenResult = ''
let tokBytes = []

for (let i = 0; i < buffer.length; i++) {
  const byte = Number(buffer[i])
  if (length === 0) {
    if (tokBytes.length > 0) {
      tiktokenResult += `${Buffer.from(tokBytes).toString('base64')} ${count}\n`
      tokBytes = []
      count += 1
    }
    length = byte
    continue;
  }
  tokBytes.push(byte)
  length--;
}
fs.writeFileSync(`${__dirname}/../data/cl100k_base.result.tiktoken`, tiktokenResult);
