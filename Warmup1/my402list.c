#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "cs402.h"

#include "my402list.h"

int  My402ListLength(My402List* pList){
 return pList -> num_members;
}

int  My402ListEmpty(My402List* pList){
 if(pList -> num_members == 0){
  return TRUE;
 }
 else{
  return FALSE;
 }
}

int  My402ListAppend(My402List *pList, void *objt ){
 My402ListElem* new = (My402ListElem*)malloc(sizeof(My402ListElem));
 if(new == NULL){
  return FALSE;
 }
 else{
  if(My402ListEmpty(pList) == TRUE) {
   pList -> anchor.next = new;
   pList -> anchor.prev = new;
   new -> next = &(pList -> anchor);
   new -> prev = &(pList -> anchor);
  }
  else{
   My402ListElem* last = My402ListLast(pList);
   last -> next = new;
   new -> next = &(pList -> anchor);
   new -> prev = last;
   pList -> anchor.prev = new;    
  }
  (pList -> num_members)++;
  new -> obj = objt;
  return TRUE; 
 }
}

int  My402ListPrepend(My402List* pList, void *objt){
 My402ListElem* new = (My402ListElem*)malloc(sizeof(My402ListElem));
 if(new == NULL){
  return FALSE;
 }
 else{
  if(My402ListEmpty(pList) == TRUE) {
   pList -> anchor.next = new;
   pList -> anchor.prev = new;
   new -> next = &(pList -> anchor);
   new -> prev = &(pList -> anchor);
  }
  else{
   My402ListElem* first = My402ListFirst(pList);
   first -> prev = new;
   new -> prev = &(pList -> anchor);
   new -> next = first;
   pList -> anchor.next = new;    
  }
  (pList -> num_members)++;
  new -> obj = objt;
  return TRUE; 
 }

}

void My402ListUnlink(My402List* pList, My402ListElem* elem){
 My402ListElem* p = elem -> prev;
 My402ListElem* n = elem -> next;
 p -> next = n;
 n -> prev = p;
 free(elem);
 (pList -> num_members)--;  
}

void My402ListUnlinkAll(My402List* pList){
 My402ListElem* first = NULL;
 while(!(My402ListEmpty(pList))){
  first = My402ListFirst(pList);
  My402ListUnlink(pList, first);
 }
}

int  My402ListInsertBefore(My402List* pList, void* objt, My402ListElem* elem){
 if(elem == NULL){
  return My402ListPrepend(pList, objt);
 }
 else if(My402ListPrev(pList, elem) == NULL){
  return My402ListPrepend(pList, objt);  
 }
 else{
  My402ListElem* new = (My402ListElem*)malloc(sizeof(My402ListElem));
  if(new == NULL){
   return FALSE;
  }
  else{
   new -> obj = objt;
   (pList -> num_members)++;
   new -> prev = elem -> prev;
   new -> next = elem;
   elem -> prev -> next = new;
   elem -> prev = new;
  }
  return TRUE;  
 }
}

int  My402ListInsertAfter(My402List* pList, void* objt, My402ListElem* elem){
  if(elem == NULL){
  return My402ListAppend(pList, objt);
 }
 else if(My402ListNext(pList, elem) == NULL){
  return My402ListAppend(pList, objt);  
 }
 else{
  My402ListElem* new = (My402ListElem*)malloc(sizeof(My402ListElem));
  if(new == NULL){
   return FALSE;
  }
  else{
   new -> obj = objt;
   (pList -> num_members)++;
   new -> prev = elem;
   new -> next = elem -> next;
   elem -> next -> prev = new;
   elem -> next = new;
  }
  return TRUE;  
 }
}

My402ListElem *My402ListFirst(My402List* pList){
 if(My402ListEmpty(pList) == TRUE) {
  return NULL;
 }
 else{ 
 return pList -> anchor.next;
 }
}

My402ListElem *My402ListLast(My402List* pList){
 if(My402ListEmpty(pList) == TRUE) {
  return NULL;
 }
 else{ 
 return pList -> anchor.prev;
 } 
}

My402ListElem *My402ListNext(My402List *pList, My402ListElem *elem){
 My402ListElem* q = My402ListLast(pList);
 if(elem == q){
  return NULL;
 }
 else{
  return elem -> next;
 }
}

My402ListElem *My402ListPrev(My402List* pList, My402ListElem *elem){
 My402ListElem* q = My402ListFirst(pList);
 if(elem == q){
  return NULL;
 }
 else{
  return elem -> prev;
 }
}

My402ListElem *My402ListFind(My402List* pList, void* objt){
 My402ListElem *elem = NULL;
 for(elem = My402ListFirst(pList); elem!=NULL; elem = My402ListNext(pList,elem)){
  if(elem -> obj == objt){
   return elem;
  }
 }
 if(elem == NULL){
  return NULL;
 }
 return NULL;
}

int My402ListInit(My402List* pList){
 pList -> num_members = 0;
 pList -> anchor.obj = NULL;
 pList -> anchor.next = pList -> anchor.prev = &(pList -> anchor);
 if(My402ListEmpty(pList) == TRUE) {
  return TRUE;
 }
 else {
  return FALSE;
 }
}


