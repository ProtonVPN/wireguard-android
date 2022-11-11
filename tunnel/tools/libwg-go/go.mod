module golang.zx2c4.com/wireguard/android

go 1.17

require (
	golang.org/x/sys v0.0.0-20220728004956-3c1f35247d10
	golang.zx2c4.com/wireguard v0.0.0-20211028114750-eb6302c7eb71
)

require (
	github.com/andybalholm/brotli v1.0.4 // indirect
	github.com/klauspost/compress v1.15.9 // indirect
	github.com/refraction-networking/utls v1.1.5 // indirect
	golang.org/x/crypto v0.0.0-20220829220503-c86fa9a7ed90 // indirect
	golang.org/x/net v0.0.0-20220909164309-bea034e7d591 // indirect
)

replace golang.zx2c4.com/wireguard => ./wireguard-go
