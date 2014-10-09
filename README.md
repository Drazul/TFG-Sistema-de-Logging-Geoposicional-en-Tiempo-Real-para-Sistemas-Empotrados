Este Trabajo Fin de Grado (TFG) se ha realizado dentro del proyecto «Biker Assistant
 (BA)», un firmware para el control de motocicletas eléctricas, desarrollado por la empresa
 SmartSoC Solutions. Dicha empresa ha impuesto unas restricciones de diseño y calidad propias,
 necesarias para una correcta y sencilla integración de este TFG dentro del proyecto que
 está desarrollando. Esta imposición ha influido de manera notoria en la toma de decisiones
 sobre el desarrollo e implementación del sistema.

 En este TFG se implementa un sistema de logging geoposicional, que almacena los datos
 recibidos por un dispositivo GPS en la memoria FLASH de la placa en la que se ejecuta el
 sistema. Durante este documento se ofrece una visión detallada de los aspectos más importantes
 a tener en cuenta en el desarrollo de cualquier sistema empotrado, de los retos que
 ofrece la programación a bajo nivel y con unas fuertes restricciones de memoria y capacidad
 de cómputo.

 Este tipo de sistemas son muy utilizados hoy en día y existen multitud de dispositivos que
 lo implementan, como puede ser cualquier smartphone. La complejidad del desarrollo de los
 sistemas empotrados viene dada por las restricciones de hardware y software impuestas por
 la empresa interesada, lo cual ha llevado a tener que tomar varias decisiones de diseño con
 el fin de adaptar la funcionalidad requerida de la aplicación a los recursos disponibles.
 Entre las restricciones del hardware impuesto por la empresa se encuentra se encuentra la
 utilización de la placa «CPU ARM Cortex M3 STMF205VGT6» y el dispositivo GPS «GlobalTop
 FGPMMOPA6H».

 El presente trabajo se puede dividir en dos objetivos bien diferenciados: 1) realizar un
 sistema de comunicación entre la placa del sistema con el componente GPS para extraer los
 datos que nos proporcione, y 2) la implementación de un sistema de archivos que nos de
 soporte para el almacenamiento y posterior lectura de los datos extraídos del GPS. Estos
 objetivos se dividen en varios subobjetivos y su diseño, desarrollo e implementación son los
 que se recogen en este documento.

 Una de las decisiones más destacables del desarrollo de este TFG que se han tenido que
 tomar está el implementar un Sistema de Archivos de desarrollo propio, ya que, como se
 verá en el apartado 5.6, el hardware presenta unas restricciones que impiden la utilización de
 un Sistema de Archivos comercial.