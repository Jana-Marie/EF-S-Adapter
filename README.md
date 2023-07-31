# EF-S-Adapter

Hard- and Firmware to control EF/EF-S lenses. The protocol used has grown since the first generation of EF(-S) lenses hence is a bit extensive with different layers of compatibility. On the physical layer a SPI like interface is used with I2S alike clock-stretching. 

**Please use at your own risk, the hard- and firmware are experimental and can damage your lens!**

![](/images/1.jpg)
![](/images/2.jpg)


### Status

##### v1
 
  - Tested
  - May shorts power rails on lens attachment
  
##### v1.1

 - Tested


### Todo

 - [x] Make lens hotplugable (needs new hardware revision)
 - [ ] Create helper functions (like set_aperture_to(aperture))
 - [ ] Documentation

### Thanks

Thanks to [Jean](https://pickandplace.wordpress.com/2011/10/05/canon-ef-s-protocol-and-electronic-follow-focus/) for this blogpost!
