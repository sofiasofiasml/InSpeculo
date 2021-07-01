<h1>InSpeculo</h1>

[![Watch the video](https://i.postimg.cc/Dwg7hKMX/In-Speculo.png)](https://www.youtube.com/watch?v=_UVJwmpZQaQ&list=LL&index=3)

<h2>Presentación</h2>

<h3>Información</h3>

Hola, somos Sofia Martínez y Patrícia Martínez y vamos a presentar nuestro juego In Speculo. 

Nuestra protagonista encuentra un pueblo perdido en la nada, cansada de tantas horas de estar caminando sin ningún rumbo decide refugiarse en el único edificio que hay en este pueblo, una iglesia. 

Menú: En esta sección podemos escuchar un audio con un loop y un fondo extraido de una parte de nuestro juego, tres botones semitransparentes que al pasar el mouse por encima cambian de color las letras. Si apretamos el botón quit, se cierra el juego, si pasamos por encima del botón de control, se visualiza la imagen de los controles de nuestro juego, donde podemos ver que para movernos, debemos hacer uso de las flechas del teclado o de las letras wasd, el shift izquierdo permitirá coger un objeto, ESC para cerrar el juego y el botón derecho del mouse para mover la cámara. Y por último si damos click derecho al botón de play, empieza el juego. 


Intro: En esta sección hemos implementado un shader con una luz direccional y con niebla, para darle un toque de misterio. También hemos generado un skybox, que no se aprecia mucho con esta niebla, pero posee una textura de cielo nublado. El jugador, puede mover la cámara a partir del ratón donde esto simula ser el cuello de la protagonista, por eso mismo hemos implementado unos límites verticales. Cuando el personaje se mueve, se puede escuchar un audio de unos pasos caminando por un terreno y durante todo el tiempo que este personaje está en este stage, va a sonar un audio con un loop. También tenemos que destacar que cuando el personaje está quieto tiene una animación diferente a la que está caminando y  el cambio entre una y la otra es progresivo. Y por último, cuando el personaje se acerca a la iglesia, las puertas se abren, iniciando la animación. 

Corridor: Podemos apreciar que tarda un poco en cargar la sala, esto es debido a una optimización de juego, para cargar cad escenario solo cuando es necesario y al salir de cada uno borramos la memoria para optimizar recursos. En esta sala el audio de los pasos es distinto ya que suena como un eco. Arriba a la izquierda podemos apreciar el gui, donde nos informa de los trozos de espejo recuperados. Estos trozos de espejo se van a ir colocando en el portal donde será el objetivo del juego. Aquí podemos apreciar un shader de antorchas. En esta sala podemos apreciar un portal y cuatro pasadizos, uno que es el que venimos, y tres que comunican con tres salas, donde cada una está relacionada con las tres características que define al ser humano: cuerpo, mente y espíritu. Debemos indicar que no hace falta entrar en un orden concreto en cada sala. 

Body o cuerpo: Al acercarnos a la puerta se inicializa la animación de esta. La finalidad de esta sala es hacer uso del cuerpo del personaje, para ello decidimos usar las colisiones, Podemos ver que el agua nos refleja el mundo espejo, en este mundo podemos visualizar objetos que en el mundo real no se pueden ver, pero si se pueden sentir. Por esa razón podemos ver que aquí estamos colisionando, ya que si miramos abajo hay un armario. Una vez llegamos al final del laberinto podemos coger el trozo de espejo a partir de las colisiones. Hemos creado un shader con agua estancada en el suelo a partir de ruido y una textura. Podemos apreciar que el audio de los pasos es diferente, al caminar sobre el agua que en otros stages. El trozo de espejo que obtenemos es de un shader de espejo y suena un ruido al cogerlo. 

Corridor: una vez el personaje vuelve, aparece en el pasillo donde entró, el trozo de espejo se coloca solo en el portal y vuelve a sonar otra vez la música característica de este stage. 

Mind, cuerpo: La sala de la mente, es un puzzle que se resuelve obteniendo pistas a partir del entorno, También debemos decir que está completamente inspirada en algunas películas de la saga de Indiana jones, concretamente en En busca del arca perdida e Indiana Jones y la última cruzada. Al acercarnos a la puerta esta se traslada progresivamente, iniciando la animación. Para superar esta sala, debemos fijarnos en los cuadros de la sala, donde podemos apreciar que el dios que coincide en los tres es Ra. Esta es la pista clave, ya que si nos fijamos en los tres objetos diferentes que son iluminados progresivamente por una luz spot, son el medallón de Ra, la cruz egipcia y el santo grial. Si miramos el mundo espejo, vemos que estos objetos son visibles, y en el mundo real, podemos interaccionar con ellos. Cuando cogemos un objeto erróneo y lo dejamos en el altar, la intensidad de la luz va disminuyendo. Al dejarlo en el altar, el objeto vuelve a aparecer en esa posición. En caso que cojamos el objeto correcto, la luz, nos indica dónde se encuentra el trozo de espejo a encontrar, si nos acercamos y apretamos el shift, lo cojemos y la animación de la puerta se activa, dejándonos volver al pasillo. 

Corridor: Volvemos al mismo  pasadizo que hemos entrado. Podemos apreciar que en el portal se ha implementado el segundo trozo de espejo. 

Soul, alma: En esta sala la finalidad es saber que el personaje se encuentra dentro de la realidad del espejo, para ello debemos darnos cuenta que las letras de la ouija o los números del altar en el mundo mirror están reflejados y en el mundo real, no. Para superar este puzzle, el objetivo es poner el planchette de la ouija sobre el símbolo que sea simétrico, que se vea igual en las dos realidades, por lo tanto es la letra M, cuando lo situamos encima de el altar el espejo que separa las realidades se rompe y es aquí donde hacemos uso de las partículas, seguidamente debemos obtenemos el trozo de cristal, que se visualiza una vez se resuelva el puzzle y la puerta se nos abre para volver al pasillo central.

Corridor: En  este momento al tener los 3 cristales la música cambia, si intenta volver a entrar en alguna sala, como ya ha sido desbloqueada, ya no puede y se genera una spot al centro del portal, si apretamos shift se nos genera el estado final.

End: Donde vemos y escuchamos un espejo romperse, los créditos. Y a partir de aquí volvemos al título y podemos volver a jugarlo.

