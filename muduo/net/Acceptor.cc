// Copyright 2010 Shuo Chen (chenshuo at chenshuo dot com)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <muduo/net/Acceptor.h>

#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/SocketsOps.h>

#include <boost/bind.hpp>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
  : loop_(loop),
    acceptSocket_(sockets::createNonblockingOrDie()),
    acceptChannel_(loop, acceptSocket_.fd()),
    listenning_(false)
{
  acceptSocket_.setReuseAddr(true);
  acceptSocket_.bindAddress(listenAddr);
  acceptChannel_.setReadCallback(
      boost::bind(&Acceptor::accept, this));
}

void Acceptor::listen()
{
  listenning_ = true;
  acceptSocket_.listen();
  acceptChannel_.set_events(Channel::kReadEvent);
  loop_->updateChannel(&acceptChannel_);
}

void Acceptor::accept()
{
  InetAddress peerAddr(0);
  int connfd = acceptSocket_.accept(&peerAddr);
  string hostport = peerAddr.toHostPort();
  printf("Connecting from %s\n", hostport.c_str());
  if (newConnectionCallback_)
  {
    newConnectionCallback_(connfd, peerAddr);
  }
  else
  {
    sockets::close(connfd);
  }
}
