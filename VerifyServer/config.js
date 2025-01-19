const fs = require('fs')

let config = JSON.parse(fs.readFileSync('config.json', 'utf-8'))
let email_user = config.email.user;
let email_pass = config.email.pass;
let mysql_host = config.mysql.host;
let mysql_port = config.mysql.port;
let mysql_password = config.mysql.password;
let redis_host = config.redis.host;
let redis_port = config.redis.port;
let redis_password = config.redis.password;
let code_prefix = "verifycode_";

module.exports = { email_pass, email_user, mysql_host, mysql_port, mysql_password, redis_host, redis_port, redis_password, code_prefix }