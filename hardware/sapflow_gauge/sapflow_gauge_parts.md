# Sapflow Gauge Parts

![sapflow gauge 3d](/docs/sapflow_gauge_3d.png)

In order to use the gauge, you'll need to:
1. Build the gauge head; and
2. Connect the gauge head to your microcontroller.

## Gauge Head Parts

Digikey links provided for convenience, but feel free to order from your favorite electronic components distributor.
| Part | Link | Quantity Needed Per Gauge |
|----------------------------------|------------------------------------------------------------------------------------------------------------------|---------------------------|
| HDC2080 Temperature & Humidity Sensor (6-WSON Package) | https://www.digikey.com/product-detail/en/texas-instruments/HDC2080DMBT/296-49482-1-ND/9360700 | 2 |
| DMN3051L-7 Logic Level Mosfet (SOT23 Package) | https://www.digikey.com/product-detail/en/diodes-incorporated/DMN3051L-7/DMN3051LDICT-ND/1964754 | 1 |
| 100 Ohm Resistor (1206 Package) | https://www.digikey.com/product-detail/en/stackpole-electronics-inc/RNCP1206FTD100R/RNCP1206FTD100RCT-ND/2240655 | 1 |
| 270 Ohm Resistor (0603 Package) | https://www.digikey.com/product-detail/en/panasonic-electronic-components/ERJ-3EKF2700V/P270HCT-ND/1746751 | 1 |
| 5K Ohm Resistor (0603 Package) | https://www.digikey.com/product-detail/en/te-connectivity-passive-product/CRGP0603F5K6/A130425CT-ND/8578257 | 2 |
| 10K Ohm Resistor (0603 Package) | https://www.digikey.com/product-detail/en/stackpole-electronics-inc/RNCP0603FTD10K0/RNCP0603FTD10K0CT-ND/2240478 | 1 |
| 0.1 uF Capacitor (0603 Package) | https://www.digikey.com/product-detail/en/samsung-electro-mechanics/CL10B104KB8NNNC/1276-1000-1-ND/3889086 | 2 |

## Gauge Head Connector Parts
You can use any method you'd like to connect the 5 pins (HEAT, 3V3, GND, SCL, SDA) from the gauge head to your microcontroller. We've had good results using RJ11 connectors with CAT3 ethernet cables up to ~10 feet (~3 meters).

| Part | Link |
|-----------------------|-------------------------------------------------------------------------------------------------|
| RJ11 Breakout Board | https://www.digikey.com/product-detail/en/sparkfun-electronics/BOB-14021/1568-1440-ND/6228638 |
| RJ11 Female Connector | https://www.digikey.com/product-detail/en/amphenol-icc-fci/54601-906002WPLF/609-5080-ND/4242890 |