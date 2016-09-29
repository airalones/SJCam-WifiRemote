#export ESP_MODULE := ESP12E 
export ESP_MODULE := ESP13

MODULES = src lib

#SPIFF_FILES = files

COM_PORT	= COM5
COM_SPEED	= 230400
SPI_SPEED   = 40

ifeq ($ESP_MODULE, ESP13)
	SPI_MODE  = qio
	SPI_SIZE  = 1M
else
	SPI_MODE  = dio
	SPI_SIZE  = 4M
endif
