// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: message.proto

#include "message.pb.h"
#include "message.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace message {

static const char* VerifyService_method_names[] = {
  "/message.VerifyService/GetVerifyCode",
};

std::unique_ptr< VerifyService::Stub> VerifyService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< VerifyService::Stub> stub(new VerifyService::Stub(channel, options));
  return stub;
}

VerifyService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_GetVerifyCode_(VerifyService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status VerifyService::Stub::GetVerifyCode(::grpc::ClientContext* context, const ::message::GetVerifyReq& request, ::message::GetVerifyRsp* response) {
  return ::grpc::internal::BlockingUnaryCall< ::message::GetVerifyReq, ::message::GetVerifyRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetVerifyCode_, context, request, response);
}

void VerifyService::Stub::async::GetVerifyCode(::grpc::ClientContext* context, const ::message::GetVerifyReq* request, ::message::GetVerifyRsp* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::message::GetVerifyReq, ::message::GetVerifyRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetVerifyCode_, context, request, response, std::move(f));
}

void VerifyService::Stub::async::GetVerifyCode(::grpc::ClientContext* context, const ::message::GetVerifyReq* request, ::message::GetVerifyRsp* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetVerifyCode_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::message::GetVerifyRsp>* VerifyService::Stub::PrepareAsyncGetVerifyCodeRaw(::grpc::ClientContext* context, const ::message::GetVerifyReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::message::GetVerifyRsp, ::message::GetVerifyReq, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetVerifyCode_, context, request);
}

::grpc::ClientAsyncResponseReader< ::message::GetVerifyRsp>* VerifyService::Stub::AsyncGetVerifyCodeRaw(::grpc::ClientContext* context, const ::message::GetVerifyReq& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetVerifyCodeRaw(context, request, cq);
  result->StartCall();
  return result;
}

VerifyService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      VerifyService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< VerifyService::Service, ::message::GetVerifyReq, ::message::GetVerifyRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](VerifyService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::message::GetVerifyReq* req,
             ::message::GetVerifyRsp* resp) {
               return service->GetVerifyCode(ctx, req, resp);
             }, this)));
}

VerifyService::Service::~Service() {
}

::grpc::Status VerifyService::Service::GetVerifyCode(::grpc::ServerContext* context, const ::message::GetVerifyReq* request, ::message::GetVerifyRsp* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


static const char* StatusService_method_names[] = {
  "/message.StatusService/GetChatServer",
  "/message.StatusService/CheckToken",
};

std::unique_ptr< StatusService::Stub> StatusService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< StatusService::Stub> stub(new StatusService::Stub(channel, options));
  return stub;
}

StatusService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_GetChatServer_(StatusService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_CheckToken_(StatusService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status StatusService::Stub::GetChatServer(::grpc::ClientContext* context, const ::message::GetChatServerReq& request, ::message::GetChatServerRsp* response) {
  return ::grpc::internal::BlockingUnaryCall< ::message::GetChatServerReq, ::message::GetChatServerRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetChatServer_, context, request, response);
}

void StatusService::Stub::async::GetChatServer(::grpc::ClientContext* context, const ::message::GetChatServerReq* request, ::message::GetChatServerRsp* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::message::GetChatServerReq, ::message::GetChatServerRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetChatServer_, context, request, response, std::move(f));
}

void StatusService::Stub::async::GetChatServer(::grpc::ClientContext* context, const ::message::GetChatServerReq* request, ::message::GetChatServerRsp* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetChatServer_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::message::GetChatServerRsp>* StatusService::Stub::PrepareAsyncGetChatServerRaw(::grpc::ClientContext* context, const ::message::GetChatServerReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::message::GetChatServerRsp, ::message::GetChatServerReq, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetChatServer_, context, request);
}

::grpc::ClientAsyncResponseReader< ::message::GetChatServerRsp>* StatusService::Stub::AsyncGetChatServerRaw(::grpc::ClientContext* context, const ::message::GetChatServerReq& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetChatServerRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status StatusService::Stub::CheckToken(::grpc::ClientContext* context, const ::message::CheckTokenReq& request, ::message::CheckTokenRsp* response) {
  return ::grpc::internal::BlockingUnaryCall< ::message::CheckTokenReq, ::message::CheckTokenRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_CheckToken_, context, request, response);
}

void StatusService::Stub::async::CheckToken(::grpc::ClientContext* context, const ::message::CheckTokenReq* request, ::message::CheckTokenRsp* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::message::CheckTokenReq, ::message::CheckTokenRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_CheckToken_, context, request, response, std::move(f));
}

void StatusService::Stub::async::CheckToken(::grpc::ClientContext* context, const ::message::CheckTokenReq* request, ::message::CheckTokenRsp* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_CheckToken_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::message::CheckTokenRsp>* StatusService::Stub::PrepareAsyncCheckTokenRaw(::grpc::ClientContext* context, const ::message::CheckTokenReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::message::CheckTokenRsp, ::message::CheckTokenReq, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_CheckToken_, context, request);
}

::grpc::ClientAsyncResponseReader< ::message::CheckTokenRsp>* StatusService::Stub::AsyncCheckTokenRaw(::grpc::ClientContext* context, const ::message::CheckTokenReq& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncCheckTokenRaw(context, request, cq);
  result->StartCall();
  return result;
}

StatusService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      StatusService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< StatusService::Service, ::message::GetChatServerReq, ::message::GetChatServerRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](StatusService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::message::GetChatServerReq* req,
             ::message::GetChatServerRsp* resp) {
               return service->GetChatServer(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      StatusService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< StatusService::Service, ::message::CheckTokenReq, ::message::CheckTokenRsp, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](StatusService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::message::CheckTokenReq* req,
             ::message::CheckTokenRsp* resp) {
               return service->CheckToken(ctx, req, resp);
             }, this)));
}

StatusService::Service::~Service() {
}

::grpc::Status StatusService::Service::GetChatServer(::grpc::ServerContext* context, const ::message::GetChatServerReq* request, ::message::GetChatServerRsp* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status StatusService::Service::CheckToken(::grpc::ServerContext* context, const ::message::CheckTokenReq* request, ::message::CheckTokenRsp* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace message

