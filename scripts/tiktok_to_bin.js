const fs = require('fs')

const tiktokenFile = fs.readFileSync(`${__dirname}/../data/cl100k_base.tiktoken`, 'utf8')

const tokrows = tiktokenFile.split('\n')
const bin = []
for (const tokrow of tokrows) {
  const [tok] = tokrow.split(' ')
  if (!tok) {
    continue;
  }
  const buffer = Buffer.from(String(tok), 'base64')
  bin.push(buffer.length)
  for (let i = 0; i < buffer.length; i++) {
    bin.push(buffer[i]);
  }
}
const buffer = Buffer.from(bin);
fs.writeFileSync(`${__dirname}/../dist/cl100k_base.bin`, buffer);

// let count = 0
// let length = 0
// let tiktokenResult = ''
// let tokBytes = []

// for (let i = 0; i < buffer.length; i++) {
//   const byte = Number(buffer[i])
//   if (length === 0) {
//     if (tokBytes.length > 0) {
//       tiktokenResult += `${Buffer.from(tokBytes).toString('base64')} ${count}\n`
//       tokBytes = []
//       count += 1
//     }
//     length = byte
//     continue;
//   }
//   tokBytes.push(byte)
//   length--;
// }
// fs.writeFileSync(`${__dirname}/../data/cl100k_base.result.tiktoken`, tiktokenResult);
