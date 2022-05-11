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

Our backups can be found <a href="https://apbe-my.sharepoint.com/personal/s120267_ap_be/_layouts/15/onedrive.aspx?id=%2Fpersonal%2Fs120267%5Fap%5Fbe%2FDocuments%2FZanzibar%20gateway%20backups&ga=1">here</a>.

You can write the image to an SD-card with you computer using the Raspberry Pi Imager.
Click on "Choose OS", scroll to the bottom, select "Use own file" and navigate to the .img or .img.gzip file. Both work.

Or you can use the Image File Utilities again to mount the image from the USB-stick straight onto the RPi.

1. Insert and mount the USB-stick: `sudo mount /dev/sda1 /mnt/Image/`
2. Mount backup: `sudo image-utils/image-mount /mnt/Image/GatewayBackup.img  MountedImages`
3. When done, run: `sudo umount MountedImages; sudo losetup -d /dev/loop0`



---