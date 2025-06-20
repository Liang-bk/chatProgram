const nodemailer = require('nodemailer')
const config_module = require('./config')

let transport = nodemailer.createTransport({
    host: 'smtp.163.com',
    port: 465,
    secure: true,
    auth: {
        user: config_module.email_user,
        pass: config_module.email_pass
    }
});

function SendMail(mailOptions) {
    return new Promise(function (resolve, reject) {
        transport.sendMail(mailOptions, function (error, info) {
            if (error) {
                console.log(error);
                reject(error);
            } else {
                console.log('邮件已成功发送: ' + info.response);
                resolve(info.response)
            }
        });
    })
}

module.exports.SendMail = SendMail;