// GetLocalIPAndMacAddr.go
package main

import (
	"errors"
	"fmt"
	"net"
)

type LocalInfo struct {
	ip  string
	mac string
}

func GetLocalIPAndMacAddr() (*LocalInfo, error) {
	ifaces, err := net.Interfaces()
	if err != nil {
		return nil, err
	}

	for _, iface := range ifaces {
		//跳过down和loop网卡
		if iface.Flags&net.FlagUp == 0 || iface.Flags&net.FlagLoopback != 0 {
			continue
		}

		addrs, err := iface.Addrs()
		if err != nil {
			return nil, err
		}

		for _, addr := range addrs {
			if ipnet, ok := addr.(*net.IPNet); ok {
				if ipnet.IP.To4() != nil {
					return &LocalInfo{
						ip:  ipnet.IP.String(),
						mac: iface.HardwareAddr.String(),
					}, nil
				}
			}
		}
	}
	return nil, errors.New("No local ip and mac")
}
