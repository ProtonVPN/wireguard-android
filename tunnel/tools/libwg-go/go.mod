module golang.zx2c4.com/wireguard/android

go 1.20

require (
	golang.org/x/sys v0.6.0
	golang.zx2c4.com/wireguard v0.0.0-20230223181233-21636207a675
)

require (
    github.com/andybalholm/brotli v1.0.4 // indirect
    github.com/klauspost/compress v1.15.9 // indirect
    github.com/refraction-networking/utls v1.1.5 // indirect
	golang.org/x/crypto v0.7.0 // indirect
	golang.org/x/net v0.8.0 // indirect
	golang.zx2c4.com/wintun v0.0.0-20230126152724-0fa3db229ce2 // indirect
)

replace golang.zx2c4.com/wireguard => ./wireguard-go
