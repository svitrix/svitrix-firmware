# Efectos

SVITRIX puede mostrar efectos donde quieras
- Notificaciones y CustomApps. Esto mostrará el efecto como la primera capa, para que aún puedas dibujar texto sobre él.
- Capa de fondo. Esto mostrará el efecto detrás de todo y en cada app. Puedes agregarlo vía Funciones ocultas.

Solo llama el nombre de tu efecto favorito.
SVITRIX envía todos los nombres de efectos una vez después de iniciar vía MQTT a stats/effects. Así puedes crear selectores externos.
También es accesible vía HTTP /api/effects

<table>
  <tr>
    <th>Nombre</th>
    <th>Efecto</th>
    <th>Nombre</th>
    <th>Efecto</th>
  </tr>
  <tr>
    <td>BrickBreaker</td>
    <td><img src="../assets/BrickBreaker.gif" style="max-height:100px;"></td>
    <td>Checkerboard</td>
    <td><img src="../assets/Checkerboard.gif" style="max-height:100px;"></td>
  </tr>
  <tr>
    <td>Fireworks</td>
    <td><img src="../assets/Fireworks.gif" style="max-height:100px;"></td>
     <td>PingPong</td>
    <td><img src="../assets/PingPong.gif" style="max-height:100px;"></td>
  </tr>
  <tr>
    <td>Radar</td>
    <td><img src="../assets/Radar.gif" style="max-height:100px;"></td>
     <td>Ripple</td>
    <td><img src="../assets/Ripple.gif" style="max-height:100px;"></td>
  </tr>
   <tr>
    <td>Snake</td>
    <td><img src="../assets/Snake.gif" style="max-height:100px;"></td>
     <td>TwinklingStars</td>
    <td><img src="../assets/TwinklingStars.gif" style="max-height:100px;"></td>
  </tr>
   <tr>
    <td>TheaterChase</td>
    <td><img src="../assets/TheaterChase.gif" style="max-height:100px;"></td>
      <td>ColorWaves</td>
    <td><img src="../assets/ColorWaves.gif" style="max-height:100px;"></td>
  </tr>
     <tr>
    <td>SwirlOut</td>
    <td><img src="../assets/SwirlOut.gif" style="max-height:100px;"></td>
    <td>SwirlIn</td>
    <td><img src="../assets/SwirlIn.gif" style="max-height:100px;"></td>
  </tr>
<tr>
    <td>LookingEyes</td>
    <td><img src="../assets/LookingEyes.gif" style="max-height:100px;"></td>
     <td>Matrix</td>
    <td><img src="../assets/Matrix.gif" style="max-height:100px;"></td>
  </tr>
           <tr>
    <td>Pacifica</td>
    <td><img src="../assets/Pacifica.gif" style="max-height:100px;"></td>
     <td>Plasma</td>
    <td><img src="../assets/Plasma.gif" style="max-height:100px;"></td>
  </tr>
               <tr>
    <td>PlasmaCloud</td>
    <td><img src="../assets/PlasmaCloud.gif" style="max-height:100px;"></td>
    <td>MovingLine</td>
    <td><img src="../assets/MovingLine.gif" style="max-height:100px;"></td>
  </tr>
  <tr>
    <td>Fade</td>
    <td><img src="../assets/Fade.gif" style="max-height:100px;"></td>
    <td>Fire</td>
    <td><img src="../assets/Fire.gif" style="max-height:100px;"></td>
  </tr>
</table>


# Configuración de Efectos
ejemplo:
```json
{
  "effect":"Plasma",
  "effectSettings":{
    "speed":3,
    "palette":"Rainbow",
    "blend":true
  }
}
```
Todas las claves de configuración son opcionales

**speed:**
Generalmente 3 como estándar. Mayor significa más rápido.
**palette:**
Una paleta de colores es un arreglo de 16 colores para crear transiciones entre colores.
Paletas integradas: `Cloud, Lava, Ocean, Forest, Stripe, Party, Heat, Rainbow`
**blend:**
Interpola entre colores, creando un amplio conjunto de tonos intermedios para transiciones de color suaves.

Valores estándar:

| Nombre del Efecto | Velocidad | Paleta | Blend |
|-------------------|-----------|--------|-------|
| Fade | 1 | Rainbow | true |
| MovingLine | 1 | Rainbow | true |
| BrickBreaker | - | - | - |
| PingPong | 8 | Rainbow | - |
| Radar | 1 | Rainbow | true |
| Checkerboard | 1 | Rainbow | true |
| Fireworks | 1 | Rainbow | true |
| PlasmaCloud | 3 | Rainbow | true |
| Ripple | 3 | Rainbow | true |
| Snake | 3 | Rainbow | - |
| Pacifica | 3 | Ocean | true |
| TheaterChase | 3 | Rainbow | true |
| Plasma | 2 | Rainbow | true |
| Matrix | 8 | - | - |
| SwirlIn | 4 | Rainbow | - |
| SwirlOut | 4 | Rainbow | - |
| LookingEyes | - | - | - |
| TwinklingStars | 4 | Ocean | false |
| ColorWaves | 3 | Rainbow | true |
| Fire | 5 | Heat | true |



# Guía de Creación de Paletas de Color Personalizadas

Esta guía te mostrará cómo crear una paleta de colores personalizada para usar con los efectos de SVITRIX.

Una paleta de colores en SVITRIX es un arreglo de 16 colores. Cada color se representa como un objeto `RGB`, que contiene componentes rojo, verde y azul.

SVITRIX usa estas paletas para crear transiciones entre colores en los efectos. Los 16 colores en la paleta no son los únicos colores que se mostrarán. En cambio, SVITRIX interpola entre estos colores para crear un amplio espectro de tonos intermedios. Esto crea transiciones de color suaves y visualmente agradables en tus efectos.

1. Crea un archivo de texto con la extensión `.txt` (por ejemplo, `soleado.txt`) en el directorio `/PALETTES/`.
2. En el archivo de texto, define 16 colores en formato hexadecimal. Cada color debe estar en una nueva línea. Un color se define en el formato `#RRGGBB`, donde `RR` es el componente rojo, `GG` es el componente verde, y `BB` es el componente azul. Cada componente es un número hexadecimal de dos dígitos (00 a FF).

Por ejemplo, una paleta soleada podría verse así:
Nota, no uses comentarios en tu archivo de paletas.

```
0000FF   // Azul profundo del cielo en el borde del horizonte
0047AB   // Cielo más claro
0080FF   // Cielo aún más claro
00BFFF   // Cielo azul claro
87CEEB   // Cielo ligeramente nublado
87CEFA   // Cielo azul claro
F0E68C   // Nubes ligeras
FFD700   // Inicio de colores del sol
FFA500   // Colores del sol más oscuros
FF4500   // Colores del sol aún más oscuros
FF6347   // Colores rojo-naranja del sol
FF4500   // Colores oscuros del sol
FFA500   // Colores brillantes del sol
FFD700   // Colores amarillo brillante del sol
FFFFE0   // Colores muy brillantes del sol
FFFFFF   // Colores blancos del sol, luz muy brillante
```

Recuerda, los colores que defines en tu paleta sirven como puntos clave en las transiciones de color. SVITRIX interpola entre estos colores, creando un amplio conjunto de tonos intermedios para transiciones de color suaves en tus animaciones. Experimenta con diferentes ubicaciones de colores en tu paleta para lograr diferentes efectos visuales. Puedes usar blend=false para no usar colores interpolados.

# Artnet (DMX)

SVITRIX soporta Artnet de forma nativa.
Para [Jinx!](http://www.live-leds.de/) puedes <a href="../svitrix_light.jnx" download>descargar esta plantilla</a>. Solo cambia la IP de ambos universos a tu IP de SVITRIX y estarás listo.

**Para cualquier otro controlador Artnet:**
Crea 2 universos con 384 canales cada uno. También agrega un nuevo diseño de matriz con 8 cadenas de 32 LEDs y posición inicial superior izquierda. Cuando comiences a enviar datos, SVITRIX detendrá su operación normal y mostrará tus datos. 1 segundo después de que dejes de enviar datos, SVITRIX volverá a la operación normal.
