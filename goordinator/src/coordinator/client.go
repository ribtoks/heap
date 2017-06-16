package main

import (
  "../common"
  "log"
  "encoding/binary"
)

type ClientChannels struct {
  addclient chan common.Socket
  healthcheckRequest chan common.Socket
  readcommondata chan common.Socket
  runcomputation chan common.Socket
  getresult chan common.Socket
  computationResults chan *common.ComputationResult
  rmclient chan *Client
}

type Client struct {
  commondata *common.DataArray
  // buffered channel operates with int32
  info chan chan interface{}
  updateTasksDone chan int64
  
  tasksCount int64
  doneTasksCount int64
  status common.ClientStatus

  ID uint32
}

func (c *Client) UpdateDoneTasks(doneTasksCount int64) {
  c.updateTasksDone <- doneTasksCount
}

func (c *Client) GetID() uint32 { return c.ID }

func (c *Client) GetStatus() interface{} { return c.status }
func (c *Client) GetStatusChannel() chan chan interface{} { return c.info }

func (c *Client) SetHealthStatus(status int64) { c.status = common.ClientStatus(status) }
func (c *Client) GetHealthReply() interface{} {
  var reply int32
  
  if c.tasksCount > 0 {
    reply = int32(c.doneTasksCount * 100 / c.tasksCount)
  } else {
    reply = 0
  }

  return reply
}

func (c *Client) SetHealthReply(update int64) {
  c.doneTasksCount = update
}

func (c *Client) GetResultsFlagChannel() chan bool { return make(chan bool) }
func (c *Client) SetGetResultsFlag() { }

func (c *Client) GetUpdateReplyChannel() chan int64 { return c.updateTasksDone }

func (c *Client) replyInit(sock common.Socket, success bool) {
  log.Printf("Replying to client. Connection was successful: %v\n", success)
  
  var status byte
  if success {
    status = 1
  } else {
    status = 0
  }

  binary.Write(sock, binary.BigEndian, status)
}

func (c *Client) RetrieveStatus() common.ClientStatus {
  hchannel := make(chan interface{})
  c.info <- hchannel
  status := <- hchannel
  return status.(common.ClientStatus)
}
