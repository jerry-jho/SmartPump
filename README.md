# SmartPump 智能浇花器

平台：ESP8266

硬件连接：

传感器：电容或者电阻型土壤湿度传感器，通过LM393等比较器转换为模拟量，通过A0读入。本工程假设1024为湿度0%，320为湿度100%。
动作器：控制电动机的继电器控制端连接到D1