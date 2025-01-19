let code_prefix = "verifycode_"

const Error = {
    Success: 0,
    RedisError: 1,
    Exception: 2,
}

module.exports = { code_prefix, Error }