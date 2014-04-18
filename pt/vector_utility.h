#ifndef PT_VECTOR_UTILITY_H
#define PT_VECTOR_UTILITY_H

#include "config_node.h"
#include "vector.h"
#include "defs.h"
#include "output_stream.h"
#include "input_stream.h"
#include "boost/lexical_cast.h"
#include "std/string.h"

namespace pt {

/// Read \a vector from \a stream. \relatesalso vector
/// \relatesalso input_stream
template<typename T, int dim>
inline void read(input_stream& stream, vector<T,dim>& vector)
{
    read_array(stream, dim, vector.data());
}

/// Write \a vector2f into \a stream. \relatesalso vector
/// \relatesalso output_stream
template<typename T, int dim>
inline void write(output_stream& stream, vector<T,dim> const& vector)
{
    for (size_t i = 0; i < dim; ++i)
    {
        write(stream, vector[i]);
    }
}

template<typename T>
std::string to_text(T const& value)
{
    return boost::str((boost::format("%1%") % value));
}

template<typename T, int dim>
std::string to_text(vector<T,dim> const& value)
{
    assert(dim > 0);
    std::string text = lexical_cast<std::string>(value[0]);

    for (size_t i = 1; i < dim; ++i)
    {
        text += ' ';
        text += lexical_cast<std::string>(value[i]);
    }

    return text;
}

template<typename T, int dim>
std::string to_string(vector<T,dim> const& value)
{
    return to_text(value);
}

template<typename T, int dim>
void write_child(config_node& config, std::string const& key,
    vector_base<T,dim> const& value)
{
    write_child(config, key, to_text(value));
}

/// config_node free function overloads.
/// \relatesalso vector
void parse_from_text(vector<float,1>& value, std::string const& text);
/// \overload \relatesalso vector
void parse_from_text(vector2f& value, std::string const& text);
/// \overload \relatesalso vector
void parse_from_text(vector3f& value, std::string const& text);
/// \overload \relatesalso vector
void parse_from_text(vector4f& value, std::string const& text);
/// \overload \relatesalso vector
void parse_from_text(vector<int,1>& value, std::string const& text);
/// \overload \relatesalso vector
void parse_from_text(vector2i& value, std::string const& text);
/// \overload \relatesalso vector
void parse_from_text(vector3i& value, std::string const& text);
/// \overload \relatesalso vector
void parse_from_text(vector4i& value, std::string const& text);

} // namespace pt

#endif
