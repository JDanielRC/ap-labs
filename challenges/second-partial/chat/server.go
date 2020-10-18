// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

var (
	entering      = make(chan client)
	leaving       = make(chan client)
	messages      = make(chan string)       // all incoming client messages
	connectDate   = make(map[string]string) //save date from when they connected
	clientUsers   = make(map[string]bool)
	connections   = make(map[string]net.Conn)
	clientChannel = make(map[string]chan string)
	admin         string
)

func broadcaster() {
	clients := make(map[client]bool) // all connected clients
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case cli := <-entering:
			clients[cli] = true

		case cli := <-leaving:
			delete(clients, cli)
			close(cli)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)

	//accepting new users
	newUser := ""
	entry := make([]byte, 100)
	_, err := conn.Read(entry)
	if err != nil {
		conn.Close()
		log.Print(err)
	}
	for i := 0; i < len(entry); i++ {
		if entry[i] != 0 {
			newUser = newUser + string(entry[i])
		} else {
			break
		}
	}
	if name, state := clientUsers[newUser]; name && state { //check for current usernames
		ch <- "irc-server > User is already on the server, try a different one."
		conn.Close()
	} else {
		ch <- "irc-server > Welcome to the Simple IRC Server"
		//save data from the new client
		clientUsers[newUser] = true
		clientChannel[newUser] = ch
		connections[newUser] = conn
		connectDate[newUser] = time.Now().Format("2006-01-02 15:04:05")

		ch <- "irc-server > Your user " + newUser + " is succesfully logged!"
		if len(clientUsers) == 1 {
			ch <- "irc-server > Congrats, you were the first user."

		}
		if admin == "" {
			ch <- "irc-server > You're the new IRC Server ADMIN"
			fmt.Println("irc-server > [" + newUser + "] was promoted as the channel ADMIN")
			admin = newUser
		}

		messages <- "irc-server > New connected user [" + newUser + "]"
		fmt.Println("irc-server > New connected user [" + newUser + "]")
		entering <- ch
		// NOTE: ignoring potential errors from input.Err()
		input := bufio.NewScanner(conn)

		//start reading input from clients
		for input.Scan() {
			command := strings.Split(input.Text(), " ")
			if !(strings.Compare("", input.Text()) == 0) { //if it's not an empty message
				if command[0] == "/user" {
					if len(command) > 1 {
						if username, online := clientUsers[command[1]]; username && online {
							ch <- "irc-server > username: " + command[1] + " IP: " + connections[command[1]].RemoteAddr().String()
						} else {
							ch <- "irc-server > User wasn't found or is not online."
						}
					} else {
						ch <- "irc-server > Incorrect usage of command, include a user along with the command."
					}
				} else if command[0] == "/users" {
					for i := range clientUsers {
						date := connectDate[i]
						ch <- "irc-server > " + i + " connected since " + date
					}
				} else if command[0] == "/time" {
					tz, ok := os.LookupEnv("TZ")
					if ok {
						ch <- "irc-server > Local Time: " + tz + " " + time.Now().Format("15:04")
					} else {
						ch <- "irc-server > Local Time: America/Mexico_City " + time.Now().Format("15:04")
					}
				} else if command[0] == "/msg" {
					if len(command) > 1 {
						if user, online := clientUsers[command[1]]; user && online {
							message := ""
							for i := 2; i < len(command); i++ {
								message += command[i] + " "
							}
							clientChannel[command[1]] <- newUser + " is sending you a message > " + message
						} else {
							ch <- "irc-server > User is offline or does not exist."
						}
					} else {
						ch <- "irc-server > Incorrect usage of /msg, include a user to send the message and a message to send."
					}
				} else if command[0] == "/kick" {
					if admin == newUser {
						if len(command) > 1 {
							if user, online := clientUsers[command[1]]; user && online {
								clientChannel[command[1]] <- "irc-server > You're kicked from this channel"
								clientChannel[command[1]] <- "irc-server > Bad language is not allowed on this channel"
								connections[command[1]].Close()
								messages <- "irc-server > [" + command[1] + "] was kicked from channel for bad language policy violation"
								fmt.Println("irc-server > [" + command[1] + "] was kicked from channel for bad language policy violation")
							} else {
								ch <- "irc-server > User " + command[1] + " does not exist or is offline"
							}
						} else {
							ch <- "irc-server > Incorrect usage of command /kick, include a valid username"
						}
					} else {
						ch <- "irc-server > You can't kick a user if you're not the server's admin"
					}
				} else if string(command[0][0]) == "/" {
					ch <- "irc-server > No such command"
				} else { //regular messages
					messages <- newUser + " > " + input.Text()
				}

			}
		}
		//stops reading input
		leaving <- ch
		messages <- newUser + " has left"
		fmt.Println("irc-server > " + newUser + " left")
		conn.Close()
		clientUsers[newUser] = false
		if admin == newUser {
			for i := range clientUsers {
				//grant admin to the first online user in the list
				if clientUsers[i] == true {
					admin = i
					clientChannel[i] <- "irc-server > You're the new IRC Server Admin"
					messages <- "irc-server > " + i + " has been promoted as the new server admin"
					break
				}
			}
			if admin == newUser {
				fmt.Println("irc-server > There's no current user to grant the admin role.")
				admin = ""
			}
		}

	}
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {
	if len(os.Args) != 5 {
		log.Fatal("Incorret usage of server.go, try with: server.go -host localhost -port [port]")
	}

	listener, err := net.Listen("tcp", os.Args[2]+":"+os.Args[4])
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("irc-server > Simple IRC Server started at %s:%s \n", os.Args[2], os.Args[4])
	go broadcaster()
	fmt.Printf("irc-server > Ready for receiving new clients\n")
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
