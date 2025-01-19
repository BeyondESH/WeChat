const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module = require('./const')
const emailModule = require('./email')
const { v4: uuidv4 } = require('uuid')
const redis_module = require('./redis')

async function GetVerifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try {
        let key = await redis_module.GetRedis(const_module.code_prefix + call.request.email)
        console.log("verifyCode is ", key)
        let uniqueId = key;
        if (uniqueId == null) {
            uniqueId = uuidv4();
            if (uniqueId.length > 6) {
                uniqueId = uniqueId.substring(0, 6)//6位验证码
            }
            let bres = await redis_module.SetRedisExpire(const_module.code_prefix + call.request.email, uniqueId, 600)//10分钟过期
            if (bres == false) {
                callback(null, {
                    email: call.request.email,
                    error: const_module.Error.RedisError
                })
                return;
            }
        }
        console.log("uniqueId is ", uniqueId)
        let text_str = '您的验证码为:' + '\n' + uniqueId + '\n' + '请三分钟内完成注册'
        //发送邮件
        let mailOptions = {
            from: '1989601704@qq.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };

        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res)

        callback(null, {
            email: call.request.email,
            error: const_module.Error.Success
        });


    } catch (error) {
        console.log("catch error is ", error)

        callback(null, {
            email: call.request.email,
            error: const_module.Error.Exception
        });
    }

}

function main() {
    var server = new grpc.Server()
    server.addService(message_proto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        console.log('grpc server started')
    })
}

main()