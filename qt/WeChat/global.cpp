#include "global.h"

std::function<void(QWidget*)> repolish=[](QWidget *w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};

extern std::function<QString(QString )> stringToSha256=[](QString str){
    QByteArray byteInput = str.toUtf8();// 将QString转换为字节数组，因为QCryptographicHash操作于字节数组
    QCryptographicHash hash(QCryptographicHash::Sha256);//创建一个QCryptographicHash对象，指定哈希算法为SHA-256
    hash.addData(byteInput);// 添加数据到hash对象中
    QByteArray hashedResult = hash.result();// 获取结果哈希值作为字节数组
    return QString(hashedResult.toHex().data());//将结果转换为十六进制格式的字符串并返回
};

QString gate_url_prefix="";
