# KT0937-STC8
In this project, the KT0937-D8 chip can be controlled to achieve basic FM and MW band reception.

STC8G1K08(16pin) was used in this program, and the hardware I2C pins was reset to P33 and P32. 

P10 of STC8G1K08(16pin) was connected to INT pin of KT0937-D8 to enable update the frequency shows on SSD1306 OLED.

P11 of STC8G1K08(16pin) was used to switch FM or AM(MW) band.

A SSD1306 OLED screen was used to show the current frequency include FM and AM.

