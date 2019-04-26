///
/// \author Björn Marx
/// \brief if you find spelling mistakes, keep them as trophy
/// \date 12/04/2019.
/// \file hash.h
///

#ifndef PSE2018TESTS_HASH_H
#define PSE2018TESTS_HASH_H

#include  "string.h"

/*!
 * \brief Hash element
 *
 * \author Björn Marx
 */
typedef struct StringHashStruct {
	string* key;	///< key of hash element
	string* value;	///< corresponding value
} Hash;

/*!
 * \brief Structure defining a listElement
 *
 * \author Björn Marx
 */
typedef struct StringHashList {
	Hash data;						///< Hash value of list element
	struct StringHashList* next;	///< next list element
} HashList;

/*!
 * \brief creates new list
 * Creates new list, but requires
 *
 * \author Björn Marx
 */
HashList* SHL_create(Hash firstElement);

/*!
 * \brief appends a new element at the end
 *
 * \author Björn Marx
 */
void SHL_append(HashList* first, Hash next);

/*!
 * \brief frees an element in the the list including it's data's Strings
 * bear in mind that this will NOT touch the *next element
 *
 * \author Björn Marx
 * 
 * \param item: the list element that shall be freed 
 */
void SHL_free_element(HashList* item);

/*!
 * \brief removes element at position
 *
 * \author Björn Marx
 * 
 * \param first pointer to fist list element
*/
//will be implemented if needed
//bool SHL_remove(HashList* first, unsigned int position);

/*!
 * \brief deletes all elements starting at first
 *
 * \author Björn Marx
 */
void SHL_remove_all(HashList* first);

/*!
 * \brief returns pointer to the *FIRST* element containing key
 * Returns pointer to the *FIRST* element containing key, will return NULL if key is not found
 *
 * \author Björn Marx
 * 
 * \param first pointer to list
 * \param key desired key to be found
 * \return pointer to *first* element with key
 */
Hash* SHL_find_key(HashList* first, string* key);

/*!
 * \brief return index'th element of HashList
 * function returning the index'th element of HashList first, it will return the last element if index is out of bound
 *
 * \author Björn Marx
 * 
 * \param first first element of list
 * \param index index of desired element 
 * \return pointer to Hash object, will return last element if index ist out of range
 */
Hash* SHL_at(HashList* first, unsigned int index);

/*!
 * \brief creates Hash with given key and value
 *
 * \author Björn Marx
 * 
 * \return copy of Hash with desired key and value
 */
Hash SH_create(string* key, string* value);

#endif //PSE2018TESTS_HASH_H
