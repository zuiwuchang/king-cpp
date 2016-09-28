package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"log"
	"net"
	"time"
)

const (
	PROTOCOL_HEADER_SIZE = 6
	PROTOCOL_CMD_STR     = 1
	PROTOCOL_BODY_OFFSET = 6
)

func main() {
	addr := "127.0.0.1:1102"
	doOnce(addr, true)
	//doOnce(addr,false)
	//doMore(addr)

}
func doMore(addr string) {
	ch := make(chan int)
	sum := 0

	count := 1000
	for i := 0; i < count; i++ {
		go doLong(addr, i, ch)
	}
	success := 0
	for {
		v := <-ch
		if v > 0 {
			success += v
			log.Printf("connect success(%v)\n", success)
		}
		sum += v
		if sum == 0 {
			break
		}
	}
}
func doLong(addr string, i int, ch chan int) {
	var c net.Conn
	c = nil
	for c == nil {
		c, _ = net.Dial("tcp", addr)
		time.Sleep(time.Millisecond)
	}

	defer c.Close()
	ch <- 1
	bytes := make([]byte, 1024)
	reader := &Reader{}
	for {
		n, e := c.Read(bytes)
		if e != nil {
			log.Println(e)
			break
		}
		reader.Write(bytes[0:n])

		for {
			msg := reader.GetMsg()
			if msg == nil {
				break
			}
			onMsgLong(c, msg)
		}
	}
	ch <- -1
}
func onMsgLong(c net.Conn, msg IMessage) {
	str := string(msg.GetBody())
	//fmt.Println("recv", str)

	if str == "this is cerberus's server" {
		writeString(c, "i want a job")
	} else if str == "what you can do" {
		writeString(c, "i'm a solider")
	} else if str == "you are cerberus soldier now" {
		writeString(c, "i'm a solider")
		//c.Close()
	}
}
func doOnce(addr string, stopserver bool) {
	now := time.Now()
	c, e := net.Dial("tcp", addr)
	if e != nil {
		log.Fatalln(e)
	}
	defer c.Close()
	log.Println("connect ", addr)
	now1 := time.Now()

	for {
		fmt.Println("send")
		writeString(c, "this is msg 1")
		time.Sleep(time.Millisecond * 1000)
	}
	writeString2(c, "this is msg 2 left", "this is msg 2 right")
	writeString3(c, "this is msg 3")

	bytes := make([]byte, 1024)
	reader := &Reader{}
	for {
		n, e := c.Read(bytes)
		if e != nil {
			log.Println(e)
			break
		}
		reader.Write(bytes[0:n])

		for {
			msg := reader.GetMsg()
			if msg == nil {
				break
			}
			onMsgOnce(c, msg, stopserver)
		}
	}
	log.Println(time.Now().Sub(now1))
	log.Println(time.Now().Sub(now))
}
func onMsgOnce(c net.Conn, msg IMessage, stopserver bool) {
	str := string(msg.GetBody())
	fmt.Println("recv", str)

	if str == "this is cerberus's server" {
		writeString(c, "i want a job")
	} else if str == "what you can do" {
		writeString(c, "i'm a solider")
	} else if str == "you are cerberus soldier now" {
		//c.Close()
		writeString(c, "you are die")
	}
}
func writeString2(c net.Conn, s0, s1 string) {
	buf := bytes.Buffer{}

	writeStringToBuffer(&buf, s0)
	writeStringToBuffer(&buf, s1)

	//send
	c.Write(buf.Bytes())

}
func writeString(c net.Conn, str string) {
	buf := bytes.Buffer{}

	writeStringToBuffer(&buf, str)

	//send
	c.Write(buf.Bytes())
}
func writeString3(c net.Conn, str string) {
	//write flag
	b32 := make([]byte, 4)
	flag := uint32(0x0000044E)
	binary.LittleEndian.PutUint32(b32, uint32(flag))
	c.Write(b32)

	//write size
	size := len(str) + PROTOCOL_HEADER_SIZE
	b16 := make([]byte, 2)
	binary.LittleEndian.PutUint16(b16, uint16(size))
	c.Write(b16)

	//write str
	c.Write([]byte(str))
}
func writeStringToBuffer(buf *bytes.Buffer, str string) {
	//write flag
	b32 := make([]byte, 4)
	flag := uint32(0x0000044E)
	binary.LittleEndian.PutUint32(b32, uint32(flag))
	buf.Write(b32)

	//write size
	size := len(str) + PROTOCOL_HEADER_SIZE
	b16 := make([]byte, 2)
	binary.LittleEndian.PutUint16(b16, uint16(size))
	buf.Write(b16)

	//write str
	buf.WriteString(str)
}
