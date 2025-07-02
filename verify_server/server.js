const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module = require('./const')
const { v4: uuidv4 } = require('uuid')
const emailModule = require('./email')
const redis_module = require('./redis')

async function GetVerifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try {
        // 获取验证码之前首先查询redis中有没有缓存
        let vCode = await redis_module.RedisGet(const_module.code_prefix + call.request.email);
        let uniqueId = vCode;
        if (vCode == null) {
            uniqueId = uuidv4();
            if (uniqueId.length > 6) {
                uniqueId = uniqueId.substring(0, 6);
            }
            // 设置缓存120秒
            let setFlag = await redis_module.RedisSetWithTime(
                const_module.code_prefix + call.request.email, 
                uniqueId,
                600
            )
            // 设置失败
            if (!setFlag) {
                callback(null, {
                    email: call.request.email,
                    error: const_module.Errors.REDISERR
                });
                return;
            }
        }
        console.log("Verify code for ", call.request.email, " is ", uniqueId);
        let text_str = '您的验证码为' + uniqueId + '请2分钟内完成注册'
        //发送邮件
        let mailOptions = {
            from: 'xquank@163.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };
        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res)
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Success
        });
    } catch (error) {
        console.log("catch error is ", error)
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Exception
        });
    }
}

function main() {
    var server = new grpc.Server()
    server.addService(message_proto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        server.start()
        console.log('grpc server started')
    })
}

main()