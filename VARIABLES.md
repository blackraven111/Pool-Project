**ОПИСАНИЕ ПЕРЕМЕННЫХ**

const GLint n = 100; // Число итераций для сфер и окружностей 
const GLfloat epsilon = 0.15; // Отступ 
const GLfloat ballRadius = 1; // Радиус шара  
const GLfloat holeRadius = ballRadius + epsilon; // Радиус лузы  
const GLfloat legRadius = 0.5 * holeRadius; // Радиус ножки  

const GLfloat tableHeight = 3 * ballRadius; // Высота поля  
const GLfloat legHeight = 3.5 * tableHeight; // Высота ножки  
const GLfloat edgeHeight = 0.7; // Высота бортика  
const GLfloat genHeight = tableHeight + edgeHeight; // Общая высота стола  

const GLfloat edgeWidth = ballRadius + 2 * epsilon; // Ширина бортика  
const GLfloat tableWidth = 10 * edgeWidth; // Ширина сукна  
const GLfloat genWidth = 2 * edgeWidth + tableWidth; // Общая ширина стола  

const GLfloat tableLength = 20 * edgeWidth; // Длина сукна  
const GLfloat genLength = 2 * edgeWidth + tableLength; // Общая длина стола
