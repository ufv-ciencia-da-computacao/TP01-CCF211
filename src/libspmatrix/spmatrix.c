#include "./include/spmatrix.h"

static void spMatrixConstructColumn(SpMatrix *spMatrix) {
  Node auxNode;
  Node iterator = spMatrix->head;
  int i;
  for (i = 2; i <= spMatrix->lin+1; i++) {
    nodeInit(&auxNode, (-1) * i, -1, 0);
    nodeSetNextBelow(&iterator, auxNode);
    iterator = nodeGetNextBelow(iterator);
  }
  nodeSetNextBelow(&iterator, spMatrix->head);
}

static void spMatrixConstructLine(SpMatrix *spMatrix) {
  Node auxNode;
  Node iterator = spMatrix->head;
  int i;
  for (i = 2; i <= spMatrix->col+1; i++) {
    nodeInit(&auxNode, -1, (-1) * i, 0);
    nodeSetNextRight(&iterator, auxNode);
    iterator = nodeGetNextRight(iterator);
  }  
  nodeSetNextRight(&iterator, spMatrix->head);
}

static void spMatrixDiscoverFirstCol(SpMatrix *spMatrix, Node *firstCol, int col) {
  Node iterator = spMatrix->head;
  
  do {
    if (abs(nodeGetColumn(iterator))-1 == col) {
      (*firstCol) = iterator;
      break;
    }
    iterator = nodeGetNextRight(iterator); 
  } while (nodeGetColumn(iterator) != -1);
}

static void spMatrixDiscoverFirstLine(SpMatrix *spMatrix, Node *firstLine, int line) {
  Node iterator = spMatrix->head;

  do {
    if (abs(nodeGetLine(iterator))-1 == line) {
      (*firstLine) = iterator;
      break;
    }
    iterator = nodeGetNextBelow(iterator); 
  } while(nodeGetLine(iterator) != -1);
}

static void spMatrixInsertCol(SpMatrix *spMatrix, Node *createdNode, int col) {
  Node iterator;
  Node prev, next;

  Node auxFirstCol;
  spMatrixDiscoverFirstCol(spMatrix, &auxFirstCol, col);

  iterator = auxFirstCol;

  while(true) {
    next = nodeGetNextBelow(iterator);

    if(nodeGetLine(iterator) > nodeGetLine(*createdNode)) {
      nodeSetNextBelow(&prev, *createdNode);
      nodeSetNextBelow(createdNode, iterator);
      break;
    }
    
    if(nodeGetLine(iterator) == nodeGetLine(*createdNode)) {
      nodeValueConcat(&iterator, *createdNode);
      nodeFree(createdNode);
      (*createdNode) = iterator;
      break;
    }

    if(nodeGetLine(next) == nodeGetLine(auxFirstCol)) {
      nodeSetNextBelow(&iterator, *createdNode);
      nodeSetNextBelow(createdNode, next);
      break;
    }

    prev = iterator;
    iterator = next;
  }
}

static void spMatrixInsertLine(SpMatrix *spMatrix, Node *createdNode, int line) {
  Node iterator;
  Node prev, next;

  Node auxFirstLine;
  spMatrixDiscoverFirstLine(spMatrix, &auxFirstLine, line);
  
  iterator = auxFirstLine;

  while (true) {
    next = nodeGetNextRight(iterator);

    if(nodeGetColumn(iterator) > nodeGetColumn(*createdNode)) {
      nodeSetNextRight(&prev, *createdNode);
      nodeSetNextRight(createdNode, iterator);
      break;
    }

    if(nodeGetColumn(iterator) == nodeGetColumn(*createdNode)) {
      /*
        Process already done on spMatrixInsertCol, cannot free node twice.
      */
      break;
    }

    if (nodeGetColumn(next) == nodeGetColumn(auxFirstLine)) {
      nodeSetNextRight(&iterator, *createdNode);
      nodeSetNextRight(createdNode, next);
      break;
    }

    prev = iterator;
    iterator = next;
  }
}

int spMatrixInit(SpMatrix *spMatrix, int lin, int col) {
  if(lin <= 0 || col <= 0) {
    return 1;
  }
  nodeInit(&spMatrix->head, -1, -1, 0);
  spMatrix->lin = lin;
  spMatrix->col = col;
  spMatrixConstructLine(spMatrix);
  spMatrixConstructColumn(spMatrix);
  
  return 0;
}

int spMatrixInsert(SpMatrix *spMatrix, int line, int col, Item value) {
  Node createdNode;
  nodeInit(&createdNode, line, col, value);

  spMatrixInsertCol(spMatrix, &createdNode, col);
  spMatrixInsertLine(spMatrix, &createdNode, line); 

  return 0;
}

/*void spMatrixToString(SpMatrix *spMatrix, char str[]) {
  Node iterator = spMatrix->head;
  // str[0] = '\0';
  // char strTemp[32];
  int i, j;
  for(i=1; i <= spMatrix->lin; i++) {
    iterator = nodeGetNextBelow(iterator);
    iterator = nodeGetNextRight(iterator);
    for(j=1; j <= spMatrix->col; j++) {
      if(nodeGetColumn(iterator) == j && nodeGetLine(iterator) == i) {
          // sprintf(strTemp, "%.2lf\t", iterator->value);
          // strcat(str, strTemp);          
          //printf("%.2lf\t", iterator->value);
          iterator = nodeGetNextRight(iterator);
      } else {
        // strcat(str, "0.00\t");
        //printf("0.00\t");
      }
    }
    //// strcat(str, "\n");
    //printf("\n");
  }  
}*/

int spMatrixFree(SpMatrix *spMatrix) {
  if(spMatrix == NULL) {
    return 1;
  }
  Node prev; 
  Node firstCol, firstLine;
  Node iterator;
  int i;
  for (i = spMatrix->col; i > 0; i--) {
    spMatrixDiscoverFirstCol(spMatrix, &firstCol, i); 
    iterator = nodeGetNextBelow(firstCol);
    while (iterator != firstCol) {
      prev = iterator;
      iterator = nodeGetNextBelow(iterator);
      nodeFree(&prev);
    } 
    nodeFree(&firstCol);
  }

  for (i = spMatrix->lin; i > 0; i--) {
    spMatrixDiscoverFirstLine(spMatrix, &firstLine, i);
    nodeFree(&firstLine);
  }
  nodeFree(&spMatrix->head);

  return 0;
}

int spMatrixLinOutOfBounds(SpMatrix *spMatrix, int line) {
  return (spMatrix->lin < line) ? 1 : 0;
}

int spMatrixColOutOfBounds(SpMatrix *spMatrix, int col) {
    return (spMatrix->col < col) ? 1 : 0;
}
