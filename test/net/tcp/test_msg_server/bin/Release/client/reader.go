package main

import (
	"bytes"
	"encoding/binary"
)

const (
	HEADER_SIZE = 6
)

type Message struct {
	data []byte
}

func (m *Message) GetBody() []byte {
	return m.data[PROTOCOL_BODY_OFFSET:]
}

type Reader struct {
	buffer bytes.Buffer
}

func (r *Reader) Write(bytes []byte) {
	r.buffer.Write(bytes)
}
func (r *Reader) GetMsg() IMessage {
	buffer := &r.buffer
	if buffer.Len() < HEADER_SIZE {
		return nil
	}

	msg_size := int(binary.LittleEndian.Uint16(buffer.Bytes()[4:]))
	if buffer.Len() < msg_size {
		return nil
	}

	bytes := make([]byte, msg_size)
	buffer.Read(bytes)

	msg := &Message{data: bytes}
	return msg
}
