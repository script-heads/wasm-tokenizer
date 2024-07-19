const express = require('express')
const app = express()
const port = 3000
app.use(express.static(__dirname + '/../dist'))
app.listen(port, () => {
  console.log(`WASM Tokenizer running at ${port}`)
})