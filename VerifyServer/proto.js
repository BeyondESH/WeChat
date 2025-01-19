const path = require('path')
const gprc = require('@grpc/grpc-js')
const protoLoader = require('@grpc/proto-loader')

const PROTO_PATH = path.join(__dirname, 'message.proto')
const packageDefinition = protoLoader.loadSync(PROTO_PATH, { keepCase: true, longs: String, enums: String, defaults: true, oneofs: true })

const protoDescriptor = gprc.loadPackageDefinition(packageDefinition)
const message_proto = protoDescriptor.message;
module.exports = message_proto;
