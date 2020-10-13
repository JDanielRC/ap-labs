// Clock2 is a concurrent TCP server that periodically writes the time.
package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"time"
)

func handleConn(c net.Conn, timezone string) {
	defer c.Close()
	for {
		_, err := time.LoadLocation(timezone)
		if err != nil {
			fmt.Printf("Error trying to use " + timezone + " timezone")
			return
		}
		_, err2 := io.WriteString(c, timezone+" "+time.Now().Format("15:04:05\n"))
		if err2 != nil {
			return // e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	if len(os.Args) != 3 && os.Args[1] != "-port" {
		fmt.Printf("Incorrect usage, use TZ= <Time Zone> clock2.go -port (port)")
		return
	}
	//fmt.Printf("arg0: " + os.Args[0] + " arg1:" + os.Args[1] + " args2:" + os.Args[2])
	listener, err := net.Listen("tcp", "localhost:"+os.Args[2])
	if err != nil {
		log.Fatal(err)
	}
	timeZone := os.Getenv("TZ")
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err) // e.g., connection aborted
			continue
		}
		go handleConn(conn, timeZone) // handle connections concurrently
	}
}
