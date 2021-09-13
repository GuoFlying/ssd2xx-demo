# SSD20X 点屏限制，判断panel能不能点
### 说明
- 判断支不支持panel主要看panel的clk有没有在SSD2X的支持范围内

### ssd20x clk的spec如下：
- TTL输出理论CLK支持范围9Mhz-75Mhz，
   - 计算方法： htotal * vtotal * fps
- MIPI DSI输出理论CLK支持范围100Mbps/lane – 1.5Gbps/lane 
   - 计算方法：H_Total*V_Total*FPS*BitsPerPixel/lane number  
   - BitsPerPixel=24(RGB888)/18(RGB666)/16(RGB565)


### 其他
- 点过7Mhz的TTL屏，所以差的不是很多的话，还是有机会能正常点屏