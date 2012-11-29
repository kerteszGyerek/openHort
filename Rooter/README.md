Ha SSH-val beleptel a rooterre:

	screen -x
	mielott tavozol:
	ctrl-a d

Az Arduino kapcsolodasahoz az alabbiak szuksegesek (BackFire 10.03)

Ha nem tudod mit kene feltenni, turd fel az Arduinot a sajat gepedre, es nezd meg `dmesg`-gel, milyen cuccokat tolt be a kernel. Ez alapjan meg tudod talalni a megfelelo csomagokat. Nalam ezekkel mukodik:

	root@OpenWrt:/# opkg list-installed | grep usb
	kmod-usb-acm - 2.6.32.27-1
	kmod-usb-core - 2.6.32.27-1
	kmod-usb-serial - 2.6.32.27-1
	kmod-usb-serial-ftdi - 2.6.32.27-1
	kmod-usb2 - 2.6.32.27-1
	kmod-usb-uhci - 2.6.32.27-1
	libusb - 0.1.12-2
	libusb-1.0 - 1.0.8-1
	usbutils - 003-1

Az `stty` hasznalatahoz kell ez a csomag:

	coreutils-stty - 8.8-1

Ha minden klappol:

	root@OpenWrt:/# dmesg | grep usb
	usbcore: registered new interface driver usbfs
	usbcore: registered new interface driver hub
	usbcore: registered new device driver usb
	usb usb1: configuration #1 chosen from 1 choice
	usbcore: registered new interface driver cdc_acm
	usb usb2: configuration #1 chosen from 1 choice
	usb usb3: configuration #1 chosen from 1 choice
	usb 2-1: new full speed USB device using uhci_hcd and address 2
	usb 2-1: configuration #1 chosen from 1 choice
	usb 2-1: USB disconnect, address 2
	usb 2-2: new full speed USB device using uhci_hcd and address 3
	usb 2-2: configuration #1 chosen from 1 choice
	usbcore: registered new interface driver usbserial
	usbcore: registered new interface driver usbserial_generic
	usbserial: USB Serial Driver core
	usb 2-2: Detected FT232RL
	usb 2-2: Number of endpoints 2
	usb 2-2: Endpoint 1 MaxPacketSize 64
	usb 2-2: Endpoint 2 MaxPacketSize 64
	usb 2-2: Setting MaxPacketSize 64
	usb 2-2: FTDI USB Serial Device converter now attached to ttyUSB0
	usbcore: registered new interface driver ftdi_sio
