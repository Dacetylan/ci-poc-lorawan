# LoRaWAN Gateway

<img src="./pictures/gateway.png" alt="Gateway photo" width="100%">

## Introduction

To setup the LoRaWAN network with The Things Network (TTN) a gateway is needed.
A LoRa gateway is the device that connects the wireless part of the network to the LoRaWAN network server (LNS).
When an end device node, in this case a sensor, transmits it data in the form of radio waves, the gateway can receive the data.
Once received the data is send to the LNS. When using TTN the data is send over internet to TTN's LNS in the cloud.
This way the data is accessible everywhere through internet given the right authentication.
In this case a Raspberry Pi 2B is used with a IMST iC880A LoRaWAN Concentrator with Basic Station, connected to TTN V3 Stack.

## What is needed

### Hardware

To setup the LoRaWAN gateway the following is needed:
- Raspberry Pi + microSD-card
- LoRaWAN Concentrator module
- Windows computer with internet connection
- Router + ethernet cable

In this example we are using:
- Raspberry Pi 2 model B
- <a href="https://wireless-solutions.de/products/lora-solutions-by-imst/radio-modules/ic880a-spi/">IMST iC880A LoRaWAN Concentrator 868MHz</a>


### Software Tools

These aren't mandatory but can be very handy and are used in this example.
- <a href="https://www.raspberrypi.com/software/">Raspberry Pi Imager</a>: a tool to flash an OS to the SD-card.
- <a href="https://git-scm.com/">Git Bash</a>: Linux based command line console for use in Windows, part of the Git version control system.
- <a href="https://nmap.org/download">Nmap Zenmap GUI</a>: to scan the local network for IP addresses.
- <a href="https://filezilla-project.org/">FileZilla</a>: FTP file transfer tool.

If you are using Linux or MacOS you can use the build in command line console when needed.
In Windows you can use the build in Windows PowerShell console or the above mentioned Git Bash. The later is a Linux based command line console which has some benefits over PowerShell.


---

## Setting up the Raspberry Pi

### Using the Raspberry Pi Imager tool flash the OS to the microSD-card.

1. Open the Raspberry Pi Imager.
2. Choose OS: choose Raspberry Pi OS Lite (32-bit).
	>> The RPi 2B needs a 32-bit OS and we don't need a GUI for this use case.
3. Choose Storage: select the microSD-card if not selected automatically.
4. Select the cog wheel to open the advanced settings.
5. Set a different host name (optional).
6. Activate SSH with password authentication by marking the checkbox.
7. Configure the username and password.
8. Configure a WiFi network to connect to automatically.
9. Close the advanced settings window and press the Write button.

When the writing has finished, insert the SD-card in the SD-card slot in the RPi and connect the power supply.
The RPi will now start automatically.
Do not disconnect the power supply while the RPi is running. This can damage the RPi and the SD-card.


### Connecting to the RPi

There are a few different option to connect to the RPi.
Because this is a headless setup, meaning you don't have a Graphical User Interface, we are using SSH in a console.


#### SSH with Hostname

The easiest way to connect to your RPi with SSH is by using the hostname.
However be aware this option might not always work due to multiple RPi's connected to the network having the same hostname.

1. Plug in the power supply and connect the RPi to a router with an ethernet cable, or if you configured the WiFi it should connect automatically.
2. Open a console and type: `ssh [username]@[hostname]`
	```bash
	ssh admin@ttn-gw
	```
3. When connecting for the first time on an ip-addres you will be asked if you want to connect. Type `yes` to add the RPi to the list of known hosts.

<img src="./pictures/ssh_fingerprint.png" alt="" width="100%">


#### SSH with ip-addres

If connecting with the hostname doesn't work, or there are multiple RPi's with the same hostname, we can also connect using the ip-addres.
To find it we are using <a href="https://nmap.org/download">Nmap Zenmap GUI</a>. (You can also use nmap in the console.)

1. Plug in the power supply and connect the RPi to a router with an ethernet cable, or if you configured the WiFi it should connect automatically.
2. On Windows, open `cmd` and run `ipconfig"` to identify what ip-address your computer received and take note.
	>>
	>> <img src="./pictures/ipconfig.png" alt="" width="100%">
3. Open Zenmap.
4. In the Command line write `nmap -p 22` and the network range your ip-address falls into.
	>> `-p 22` scan port 22 which is the port used for SSH.
	>> Example: my computer got ip-address 192.168.100.170
	>> So the nmap command is: `nmap -p 22 192.168.100.0/24`
	>>
	>> <img src="./pictures/nmap.png" alt="" width="50%">
5. Identify the RPi in the scan results.
	>> With the MAC addresses you can identify any RPi's in the network because it starts with the manufacturers identification.
	>> Zenmap does this for us and says which is a RPi.
	>> The ip-address is given above the MAC address! Write down both addresses. We will need the MAC address later on.
6. Now that we have the ip-address, open a Git Bash console.
7. Type the command: `ssh [username]@[ip-addres]`
	>> In our example: `ssh admin@192.168.100.102`
8. When connecting for the first time on an ip-addres you will be asked if you want to connect. Type `yes` to add the RPi to the list of known hosts.


Congratulations! You ar now connected to your Raspberry Pi and can start using it.
To make things easier in the future you can also set a static ip-addres for your RPi.
This way the ip-addres is always the same. You can follow <a href="https://www.makeuseof.com/raspberry-pi-set-static-ip/">this guide</a> to set it up.

TIP: It is recommanded to run an update every time you start your RPi.
To do this type the following command: `sudo apt-get update && sudo apt-get -y upgrade`


---

## Configuring the Gateway

To configure the RPi as a gateway we are using a IMST iC880A LoRaWAN Concentrator and The Things Network.
Currently TTN is on its V3 Stack. This uses a secured and encrypted connection to it's LNS for added security.
For this purpose Semtech provides open-source software called Basic Station.
Basic Station exchanges data as JSON encoded objects over secure WebSockets.  The gateway identifies itself with a secret API key, ensuring it’s authorised.
In addition to security, the protocol allows for LNS discovery and load sharing and also includes CUPS or Configuration and Update Server allowing for easy management of gateway fleets.

>> This section is largely based on <a href="https://www.beyondlogic.org/lorawan-upgrading-to-basic-station-and-the-things-network-v3-stack/">LoRaWAN: Upgrading to Basic Station and The Things Network V3 Stack by Craig Peacock</a>


### Installation

We are going to clone Basic Station from its GitHub repository and build and executable on our target RPi.

1. Connect to your RPi console: `ssh admin@192.168.100.102`
2. Run an update: `sudo apt-get update && sudo apt-get -y upgrade`
3. Clone the repository: `git clone https://github.com/lorabasics/basicstation.git`
4. Open the created directory: `cd basicstation`
5. Build the executable: `make platform=rpi variant=std`

During the build process, dependencies such as mbed TLS and the libloragw SX1301/SX1302 Driver/HAL are downloaded and compiled.


### Configuration

The RPi communicates with the Concentrator over SPI. However this is disable by default.
The SPI interface can be enable using the Software Configuration Tool, raspi-config.
Do note the available options and where they are located in the menu can vary by version.

1. Go to the RPi configuration screen: sudo raspi-config
2. Select 3 Interface Options with the Up/Down keys.
	>>
	>> <img src="./pictures/raspi-config_3.png" alt="" width="100%">
3. Select I4 SPI and when promted select "Enable".
	>>
	>> <img src="./pictures/raspi-config_I4.png" alt="" width="100%">
4. Also under 3 Interface Options, select I6 Serial Port.
	>>
	>> <img src="./pictures/raspi-config_I6.png" alt="" width="100%">
5. When prompted answer "No" to disable shell messages, but leave the serial port enabled by answering "Yes" on the second prompt.
6. Select "Finish" with the Left/Right keys to exit raspi-config.

To be able to connect to TTN we need a couple of configuration files.
To keep them together create a new folder called TTN in the home directory.

1. `cd ~`
2. `mkdir TTN`
3. `cd TTN`


---
#### tc.uri

The first thing Basic Station needs to know is which LNS to connect with.
Use <a href="https://console.cloud.thethings.network/">The Things Network Cluster Picker</a> to determine the closest geographically located server to minimize network latency.
For Europe and Africa the best current option is Europe 1: eu1.cloud.thethings.network
After comes the port number. Packet transport with the The Things Network V3 LNS happens on port 8887.
To create the file:

1. To create the file and open it in the nano editor type: `nano tc.uri`
2. In nano type: `wss://eu1.cloud.thethings.network:8887`
3. Press Ctrl+O and Enter to save the file.
4. Press Ctrl+X to exit the nano editor.


---
#### tc.trust

Next we need to establish a trust relationship. This involves copying the root certificate (expires Jun 2035) to a file called tc.trust.

1. `curl https://letsencrypt.org/certs/isrgrootx1.pem.txt -o tc.trust`


---
#### tc.key

We also need an API key from TTN for authorisation by registering the gateway.
For this we need the gateway/station's EUI, which is it's MAC address with `FF:FE` added in the middle.
In our example the MAC address is `B8:27:EB:4D:56:2F`, so the EUI is `B8:27:EB:FF:FE:4D:56:2F`.

Now that we have the EUI we can head over to the TTN Console for registration.

1. Open a browser on your computer and go to: https://eu1.cloud.thethings.network/console
2. On the login page login to your account or create a new user by clicking on "Register".
	>>
	>> <img src="./pictures/TTNConsole_login.png" alt="" width="100%">
	>> ---
	>> <img src="./pictures/TTNConsole_create-user.png" alt="" width="100%">
3. Once logged in, select "Go to gateways".
	>>
	>> <img src="./pictures/TTNConsole_start.png" alt="" width="100%">
4. If you are a collaborator on any gateways you can see them on this page. To add a new one select "Add gateway" on the top right.
	>>
	>> <img src="./pictures/TTNConsole_gateway.png" alt="" width="100%">
5. Fill in a Gateway ID of your choice. Make sure it is easily identifiable by this name.
6. Fill in the Gateway EUI.
7. Fill in a name. This is only as heading in the console.
8. Scroll down to Frequency plan under LoRaWAN options and select the frequency your end devices/nodes will work on. In this example this is "Europe 863-870 MHz (SF9 for RX2 - recommended)
9. The other settings can be left on default. Click on "Create gateway" to finish the registration.
	>> <img src="./pictures/TTNConsole_gateway-add.png" alt="" width="100%">

Your gateway is now registered, but we still need to get an API key for authorisation.

1. In your browser go to: https://eu1.cloud.thethings.network/console/gateways
2. Select your gateway.
3. On the "Overview" page, select "API keys" on the left side panel.
	>>
	>> <img src="./pictures/TTNConsole_gateway-overview.png" alt="" width="100%">
4. Click on "Add API key" on the top right to create a new key.
	>>
	>> <img src="./pictures/TTNConsole_gateway-apikeys.png" alt="" width="100%">
5. Give the key a name.
6. And add at least the following right: "Link as Gateway to a Gateway Server for traffic exchange, i.e. write uplink and read downlink".
	>>
	>> <img src="./pictures/TTNConsole_gateway-apikeys-add.png" alt="" width="100%">
7. Click on "Create API key".
8. In the following window make sure to copy the key and write it down somewhere. You will not be able to get it later on, though you can always create a new one.
	>> <img src="./pictures/TTNConsole_gateway-apikeys-copy.png" alt="" width="100%">

Now head back over to the Git Bash console and enter the following commands:

1. `export LNS_KEY="your-API-key-here"`
2. `echo "Authorization: Bearer $LNS_KEY" | perl -p -e 's/\r\n|\n|\r/\r\n/g' > tc.key`


---
#### station.conf

Finally create the station's configuration file.

1. To create the file: nano station.conf
2. Add the following code to the file:
```bash
{
   "radio_conf": {                  /* Actual channel plan is controlled by the server */
       "lorawan_public": true,      /* is default */
       "clksrc": 1,                 /* radio_1 provides clock to concentrator */
       "device": "/dev/spidev0.0",  /* default SPI device is platform specific */
       "pps": true,
       "radio_0": {
           /* freq/enable provided by LNS - only hardware-specific settings are listed here */
           "type": "SX1257",
           "rssi_offset": -166.0,
           "tx_enable": true
       },
       "radio_1": {
           "type": "SX1257",
           "rssi_offset": -166.0,
           "tx_enable": false
       }
       /* chan_multiSF_X, chan_Lora_std, chan_FSK provided by LNS */
   },
   "station_conf": {
     "log_file":    "stderr",
     "log_level":   "DEBUG",
     "log_size":    10e6,
     "log_rotate":  3
   }
}
```

You can find out more about what this file does in <a href="https://lora-developers.semtech.com/build/software/lora-basics/lora-basics-for-gateways/?url=conf.html">the official Semtech documentation.</a>

The only part you are likely to want to change is the log settings. Instead of logging to file, we send the output to stderr for debugging.
The log levels are: XDEBUG, DEBUG, VERBOSE, INFO, NOTICE, WARNING, ERROR, CRITICAL
After the gateway works satisfactory you can choose a log level further down the line with less messages to minimise ware on the flash memory.

That concludes the configuration of the gateway.


---

### Reset

The Concentrator we are using makes use of the SX1301 Digital Baseband chip.
To work this chip needs to be reset on power-up through GPIO. We will use a script from <a href="https://github.com/Lora-net/lora_gateway/blob/master/reset_lgw.sh">mcoracin</a> to do this.
In this example the Concentrator's reset pin is connected, through a 3rd party adapter, to pin 22 of the RPi, which is GPIO 25.
This can be different depending on the Concentrator/HAT or adapter you are using, so make sure its the right one.
If when running Basic Station you get an error concerning the concentrator it is very likely you have the wrong GPIO number.
We, for instance, had some difficulty with finding the correct one.


---

#### reset_lgw.sh

1. In the TTN folder create the new file: `nano reset_lgw.sh`
2. Copy the code into the nano editor.

```bash
#!/bin/sh

# This script is intended to be used on IoT Starter Kit platform, it performs
# the following actions:
#       - export/unpexort GPIO7 used to reset the SX1301 chip
#
# Usage examples:
#       ./reset_lgw.sh stop
#       ./reset_lgw.sh start

# The reset pin of SX1301 is wired with RPi GPIO7
# If used on another platform, the GPIO number can be given as parameter.
if [ -z "$2" ]; then 
    IOT_SK_SX1301_RESET_PIN=25
else
    IOT_SK_SX1301_RESET_PIN=$2
fi

echo "Accessing concentrator reset pin through GPIO$IOT_SK_SX1301_RESET_PIN..."

WAIT_GPIO() {
    sleep 0.1
}

iot_sk_init() {
    # setup GPIO 7
    echo "$IOT_SK_SX1301_RESET_PIN" > /sys/class/gpio/export; WAIT_GPIO

    # set GPIO 7 as output
    echo "out" > /sys/class/gpio/gpio$IOT_SK_SX1301_RESET_PIN/direction; WAIT_GPIO

    # write output for SX1301 reset
    echo "1" > /sys/class/gpio/gpio$IOT_SK_SX1301_RESET_PIN/value; WAIT_GPIO
    echo "0" > /sys/class/gpio/gpio$IOT_SK_SX1301_RESET_PIN/value; WAIT_GPIO

    # set GPIO 7 as input
    echo "in" > /sys/class/gpio/gpio$IOT_SK_SX1301_RESET_PIN/direction; WAIT_GPIO
}

iot_sk_term() {
    # cleanup GPIO 7
    if [ -d /sys/class/gpio/gpio$IOT_SK_SX1301_RESET_PIN ]
    then
        echo "$IOT_SK_SX1301_RESET_PIN" > /sys/class/gpio/unexport; WAIT_GPIO
    fi
}

case "$1" in
    start)
    iot_sk_term
    iot_sk_init
    ;;
    stop)
    iot_sk_term
    ;;
    *)
    echo "Usage: $0 {start|stop} [<gpio number>]"
    exit 1
    ;;
esac

exit 0
```

3. Save and exit using Ctrl+O and Ctrl+X.
4. To run this file needs root permissions, to add these run: `chmod +x reset_lgw.sh`
5. Now to perform the reset: `sudo ./reset_lgw.sh start 25`


---

### Automation and Testing

To make it easier to perform tests and make the Gateway start automatically at boot we can use a start script.
We are going to use a bash script and make it run at boot using crontab.


#### start.sh

1. In the TTN folder create the new file: nano start.sh
2. Add the following code to the script:

```bash
#!/bin/bash
apt-get update && apt-get -y upgrade
/home/admin/TTN/reset_lgw.sh start 25
/home/admin/basicstation/build-rpi-std/bin/station --home=/home/admin/TTN --log-file=station.log --daemon
```

	>> This script first performs an update, so we don't have to do this ourselves.
	>> Afterwards it will perform a reset and start Basic Station as a daemon which logs to the file `station.log`.
	
3. Add root permissions to the script: `chmod +x start.sh`
4. To open/create a crontab file as root type: `sudo crontab -e`
5. At the bottom of the file add: `@reboot sudo /home/admin/TTN/start.sh`
6. Reboot the RPi and the script should run automatically: `sudo reboot`
7. When rebooted and reconnected through SSH you can read the log file with: `sudo tail -f /home/admin/TTN/station.log`
8. To escape the logging press Ctrl+C.

You can also check if the gateway is connected to TTN in the <a href="https://eu1.cloud.thethings.network/console/gateways">TTN Console's Gateways page</a>.


---

### Troubleshooting

Some errors you might run into:

- [AIO:ERRO] Recv failed: SSL - The peer notified us that the connection is going to be closed
	>> This error indicates a connection or authorisation issue when connecting to TTN. Most likely a problem with your gateway's API key.
	>> Double check the "tc.key" file, or delete the file and create a new one.
	
- [RAL:ERRO] Concentrator start failed: lgw_start
	>> This error indicates a problem with starting the concentrator. Most likely the reset isn't performed correctly.
	>> The GPIO pin you specified for the reset script could be incorrect, or the reset script itself isn't run.
	>> Double check the GPIO number and run the reset script manually to pinpoint the problem.
	>> You can easily run the reset script manually with different GPIO numbers by changing it in the arguments when running the script.
	
Other issues:
- Is SPI enabled in raspi-config?
- Correct SPI device (/dev/spidev0.0) specified in the "station.config" file?
- Does the LoRa concentrator module have enough power?


---

## Making a backup image of the SD-card

In case something goes wrong or the SD-card gets damaged by a power outage it can be very time consuming to have the reconfigure the gateway from scratch again.
For this reason it could be handy to have a backup of the fully configured and working gateway.
Just flashing the SD-card with your computer doesn't always work correctly however.
We tried making images with Win32DiskImager, and a few other tools on MacOS, but after flashing to a new SD-card the RPi wouldn't boot with these.
Thankfully there is another option. It's a tool run on the RPi itself which makes an images of the RPi it's running on and writes it to a different partition, like a USB-stick or a Network drive.

The tool is called <a href="https://forums.raspberrypi.com/viewtopic.php?p=1528736">Image File Utilities by RonR</a> who posted it on the official Raspberry Pi forums with a small guide.
You can download it <a href="https://forums.raspberrypi.com/download/file.php?id=53282&sid=95d32f51b85506293ed2bc162b2f42fb">here</a>.
A short guide and a few changes can be found <a href="https://raspberrypi.stackexchange.com/questions/103990/how-to-backup-shrink-the-image-of-my-working-raspbian-sd-card/103991#103991">here</a> by Milliways.


### Create a backup

1. Download the tool, extract and transfer it onto the RPi. This can be done using FileZilla.
2. Make all script executable: `chmod +x image-backup image-chroot image-info image-set-partuuid image-check image-compare image-mount image-shrink`
3. Format the USB-stick in exFAT and insert in the RPi.
4. Mount USB-stick: `sudo mount /dev/sda1 /mnt/Image/`
5. Start initial backup: `sudo image-utils/image-backup`
6. Image file to create?: `/mnt/Image/GatewayBackup.img`
7. Initial image file ROOT filesystem size (MB) [2205]?: This can be 0 for smallest possible or blank for full backup.
8. Added space for incremental updates after shrinking (MB) [0]?: Leave empty.
9. Create /mnt/Image/BusterBackup.img (y/n)?: `y`
10. After the backup is done you can make it smaller by compressing it: `sudo gzip -9kN /mnt/Image/GatewayBackup.img`

To make an incremental backup later on you can use: `sudo image-utils/image-backup /mnt/Image/GatewayBackup.img`


### Restoring a backup

You can write the image to an SD-card with you computer using the Raspberry Pi Imager.
Click on "Choose OS", scroll to the bottom, select "Use own file" and navigate to the .img or .img.gzip file. Both work.

Or you can use the Image File Utilities again to mount the image from the USB-stick straight onto the RPi.

1. Insert and mount the USB-stick: `sudo mount /dev/sda1 /mnt/Image/`
2. Mount backup: `sudo image-utils/image-mount /mnt/Image/GatewayBackup.img  MountedImages`
3. When done, run: `sudo umount MountedImages; sudo losetup -d /dev/loop0`

https://behind-the-scenes.net/sharing-a-raspberry-pi-directory-on-a-local-area-network/


---

## Sources
- <a href="https://www.beyondlogic.org/lorawan-upgrading-to-basic-station-and-the-things-network-v3-stack/">LoRaWAN: Upgrading to Basic Station and The Things Network V3 Stack by Craig Peacock</a>
- <a href="https://www.trell.se/blog/building-lorawan-gateway/">Building a LoRaWAN gateway using a Raspberry Pi by trell technologies</a>
- <a href="https://github.com/Lora-net/lora_gateway/blob/master/reset_lgw.sh">Reset script</a>
- <a href="https://www.raspberrypi.com/documentation/computers/configuration.html>Official Raspberry Pi documentation</a>
- <a href="https://github.com/lorabasics/basicstation">Semtech Basic Station GitHub repository</a>
- <a href="https://doc.sm.tc/station/">Semtech Basic Station official documlentation</a>
- <a href="https://forums.raspberrypi.com/viewtopic.php?p=1528736">Image File Utilities by RonR</a>
- <a href="https://raspberrypi.stackexchange.com/questions/103990/how-to-backup-shrink-the-image-of-my-working-raspbian-sd-card/103991#103991">Image File Utilities user guide by Milliways</a>

