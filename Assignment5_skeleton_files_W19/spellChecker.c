#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
    char* word = nextWord(file);
    while(word != NULL)
    {
      hashMapPut(map, word, 1);
      free(word);
      word = nextWord(file);
    }
}

/**
 * Calculates the Levenshtein Distance Between two strings.
 * @param string 1 to compare
 * @param string 2 to compare
 */

 int calcLeven(char* string1, char* string2)
 {
   int sum = 0;
   char* toUse;
   char* other;
   if(strlen(string1) <= strlen(string2))
   {
     toUse = string1;
     other = string2;
   }
   else
   {
     toUse = string2;
     other = string1;
   }
   for(int i = 0; i < strlen(toUse); i++)
   {
     if(toUse[i] != other[i])
     {
       sum++;
     }
   }
   sum += (strlen(other) - strlen(toUse));

   return sum;

 }

 /**
  * Changes all of the value pairs to be the Levenshtein Distance between the input.
  * @param Hashmap to change
  * @param String to use in calculation
  */
 void changeWeight(HashMap* map, char* comparedString)
 {
   for(int i = 0; i < map->capacity; i++)
   {
       HashLink* lnk = map->table[i];
       if (lnk != NULL)
       {
           while (lnk != NULL)
           {
               lnk->value = calcLeven(lnk->key, comparedString);
               lnk = lnk->next;
           }
       }
   }
 }

 /**
  * Returns an array of length 5 that has words with the lowest Levenshtein Distance
  * @param Map to search
  */
 char** recomend(HashMap* map)
 {
   char** arr;
   int size = 0;
   int compareLen = 1;
   while(size < 5)
   {
     for(int i = 0; i < map->capacity; i++)
     {
         HashLink* lnk = map->table[i];
         if (lnk != NULL)
         {
             while (lnk != NULL)
             {
                 if(lnk->value == compareLen)
                 {
                   if(size >= 5)
                   {
                     break;
                   }
                   arr[size] = lnk->key;
                   size++;
                 }
                 lnk = lnk->next;
             }
         }
     }
     compareLen++;
   }
   return arr;
 }


/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement


    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        // Implement the spell checker code here..

        char** poss;
        if(hashMapContainsKey(map, inputBuffer))
        {
          printf("The inputted word %s was spelled correctly.", inputBuffer);
        }
        else
        {
          changeWeight(map, inputBuffer);
          poss = recomend(map);
        }

        for(int i = 0; i < 5; i++)
        {
          printf("%s, ", poss[i]);
          printf("\n");
        }

        // for(int i = 0; i < map->capacity; i++)
        // {
        //     HashLink* lnk = map->table[i];
        //     if (lnk != NULL)
        //     {
        //         printf("\nBucket %d -> ", i);
        //         while (lnk != NULL)
        //         {
        //             printf("(%s: %d) -> ", lnk->key, lnk->value);
        //             lnk = lnk->next;
        //         }
        //         printf("NULL");
        //     }
        // }
        // printf("\n");

        file = fopen("dictionary.txt", "r");
        loadDictionary(file, map);
        fclose(file);


        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }

    hashMapDelete(map);
    return 0;
}
