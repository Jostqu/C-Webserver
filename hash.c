#include "hash.h"

HashList* SHL_create(Hash firstElement)
{
	HashList* hl = calloc(1, sizeof(HashList));

	if (hl == NULL)
		exit(1);

	hl->data = firstElement;

	return hl;
}

void SHL_append(HashList* first, Hash next)
{
	HashList* b = first;
	for (; b->next != NULL; b = b->next);

	HashList* newElement = calloc(1, sizeof(HashList));

	if (newElement == NULL)
		exit(2);

	newElement->data = next;
	newElement->next = NULL; //should be null anyways because of calloc, but safety first

	b->next = newElement;
}

void SHL_free_element(HashList* item)
{
	string_free(item->data.key);
	string_free(item->data.value);
	item->next = NULL;
	free(item);
}

void SHL_remove_all(HashList* first)
{
	HashList* element = first, *buffer;

	while (element->next != NULL)
	{
		buffer = element->next;
		SHL_free_element(element);
		element = buffer;
	}

	SHL_free_element(element);
}

Hash* SHL_find_key(HashList* first, string* key)
{
	HashList* b = first;

	do
	{
		if (string_compare(b->data.key, key))
			return &b->data;
		else
			b = b->next;
	}
	while (b);

	return NULL;
}

Hash SHL_at(HashList* first, unsigned int index)
{
	HashList* b = first;

	while (index-- > 0 && b->next != NULL)
		b = b->next;
	
	return b->data;
}

Hash SH_create(string* key, string* value)
{
	Hash hash = { key, value };
	return hash;
}

size_t SHL_get_size(HashList *first) {
    int i = 1;

    //count elements
    for (HashList* b = first; b->next; b = b->next)
        i++;

    return i;
}

Hash *SHL_find_key_cstr(HashList *first, char *key) {

    string* strKey = string_new_from_cstr(key);

    Hash* b = SHL_find_key(first, strKey);
    string_free(strKey);

    return b;
}
