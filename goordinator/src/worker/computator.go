package main

import "../common"

type Computator interface {
  beginSession(task *common.Task) error
  // implementation is supposed to return empty computation result in case of error
  computeTask(task *common.Task) (common.ComputationResult, error)
  endSession() error
}
