# MCP4725-Driver
 A DAC Converter(MCP4725) Driver for STM32.

*Note: This project depends on HAL Driver.*



**How to use:**

1. Open STM32CubeIDE
2. Create a project
3. Enable I2C Bus in your project, either one is ok.
4. In Project Manager->Code Generator,  check the "Generate peripheral initialization as a pair of '.c/.h files per peripheral" option.
5. Save and generate code.
6. Add mcp4725.c/.h to /Core/Src(and /Core/Inc)
7. Write your code!