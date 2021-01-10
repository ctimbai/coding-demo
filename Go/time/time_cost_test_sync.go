/******************************************************************************
 *	File Name: synctest.go
 *	Created Time: Tue 05 Jan 2020
 *****************************************************************************/

package main

import "fmt"
import "time"
import "sync"

var (
    sum int
    mutex sync.RWMutex
)

// @brief: 时间统计
func timeCost() func() {
    start := time.Now()
    return func() {
        tc := time.Since(start)
        fmt.Printf("time cost = %v\n", tc)
    }
}

func main() {
    defer timeCost()() // 统计后面运行时间
    run()
    fmt.Printf("total = %d\n", sum)
}

func run() {
    var wg sync.WaitGroup
    wg.Add(110)
    for i := 0; i < 100; i ++ {
        go func() {
            defer wg.Done()
            add(10)
        }()
    }
    for i := 0; i < 10; i ++ {
        go func() {
            defer wg.Done()
            readSum()
        }()
    }

    wg.Wait()
}

func add(i int) {
    mutex.Lock()
    defer mutex.Unlock()
    sum += i
}

func readSum() {
    mutex.RLock()
    defer mutex.RUnlock()
    fmt.Printf("sum = %d\n", sum)
    
}
