#ifndef SPARSER_H_04112009
#define SPARSER_H_04112009


// See sparser_set_callback() for a description of these events.
typedef enum 
{
   sparser_error,
   sparser_elem_begin,
   sparser_elem_param,
   sparser_elem_end
} sparser_action_t;

// The type of the sparser callback.
typedef int (*sparser_callback_t)(
	void *user_data,
	sparser_action_t action, 
	const char *symbol_begin, int symbol_len);

// The main structure used by the parser.
typedef struct
{
   sparser_callback_t callback;
   char *whitespace;
   void *user;
} sparser_t;

// Initialize a sparser_t structure to sane default values. Always call this 
// function before calling any other function with a sparser_t object.
//------------------------------------------------------------------------------
void sparser_init(sparser_t *sparser);

// Set the callback-function that will be called for every parse-event. 
// Parse-events are defined in the sparser_action_t enum. Besides the 
// sparse_error event, which will be called upon any error, there are only 
// three important parse-events: sparser_elem_begin is called when a new 
// element begins. That is, immediately after the element's name has been 
// parsed.
// sparser_elem_param is called immediately after a parameter has been parsed.
// sparser_elem_end is called after an element has been closed. For convenience, 
// a pointer to the element's name is passed to the callback-function. 
// This is the same as the corresponding sparser_elem_begin event.
//------------------------------------------------------------------------------
void sparser_set_callback(sparser_t *sparser, sparser_callback_t callback);

// Set the characters that are considered whitespace. Defaults to " \t" in 
// sparser_init().
//------------------------------------------------------------------------------
void sparser_set_whitespace(sparser_t *sparser, const char *whitespace);

// Set the user-data for this sparser object. The user-data is passed to 
// each call of the callback set by sparser_set_callback().
//------------------------------------------------------------------------------
void sparser_set_user_data(sparser_t *sparser, void *user_data);

// Start the parsing. See sparser_set_callback on how to actually obtain the 
// parsed values. 0 is returned upon success, an error-code upon failure. Use 
// the callback and the sparser_error event to get a detailed error-description. 
// sparser has to point to an initialized sparser structure. expression has to 
// point to a pointer to the beginning of a string.
//------------------------------------------------------------------------------
int sparser_parse(sparser_t *sparser, const char **expression);

// Deinitialize the parser. This frees memory previously allocated 
// by sparser_init().
//------------------------------------------------------------------------------
void sparser_deinit(sparser_t *sparser);

#endif