#pragma once

#include <iostream>
#include <iomanip>
#include <ios>
#include <vector>
#include <tuple>
#include <type_traits>
#include <cassert>
#include <cmath>

/**
 * Used to specify the column format
 */
enum class VarTableColumnFormat
{
    AUTO,
    SCIENTIFIC,
    FIXED,
    PERCENT
};

/**
 * Used to specify the print style
 */
enum class PrintStyle
{
    BASIC,
    SIMPLE,
    EMPTY,
    FULL
};

/**
 * Used to specify the alignment style
 */
enum class AlignmentStyle
{
    LEFT,
    RIGHT,
    INTERNAL
};

/**
 * A class for printing a table on Shell.
 *
 * Requries C++11 (and nothing more)
 *
 * You can use the header file below:
 *
 * VarTable<std::string, double, int, std::string> vt({"Name", "Weight", "Age", "Brother"});
 * vt.addRow("Fred", 193.4, 35, "Sam");
 * vt.print(std::cout);
 */
template <class... Ts>
class VarTable
{
public:
    /// The type stored for each row
    typedef std::tuple<Ts...> DataTuple;

    /**
     * Construct the table with headers
     *
     * @param headers The names of the columns
     * @param static_column_size The size of columns that can't be found automatically
     */
    VarTable(std::vector<std::string> headers,
        unsigned int static_column_size = 0,
        unsigned int cell_padding = 1)
        : _headers(headers),
        _num_columns(std::tuple_size<DataTuple>::value),
        _static_column_size(static_column_size),
        _cell_padding(cell_padding),
        _print_style(PrintStyle::BASIC),
        _alignment_style(std::tuple_size<DataTuple>::value, AlignmentStyle::LEFT)
    {
        assert(headers.size() == _num_columns);
    }

    /**
     * Add a row of data
     *
     * @param data A Tuple of data to add
     */
    void addRow(Ts... entries) { _data.emplace_back(std::make_tuple(entries...)); }

    /**
     * Pretty print the table of data
     */
    template <typename StreamType>
    void print(StreamType& stream)
    {
        size_columns();

        // Start computing the total width
        // First - we will have _num_columns + 1 "|" characters
        unsigned int total_width = _num_columns + 1;

        // Now add in the size of each colum
        for (auto& col_size : _column_sizes)
            total_width += col_size + (2 * _cell_padding);

        // Print out the top line
        if (_print_style != PrintStyle::SIMPLE && _print_style != PrintStyle::EMPTY)
            print_plus(stream);

        // Print out the headers
        if (_print_style != PrintStyle::SIMPLE && _print_style != PrintStyle::EMPTY)
            stream << "|";
        else
            stream << " ";
        for (unsigned int i = 0; i < _num_columns; i++)
        {
            // Must find the center of the column
            auto half = _column_sizes[i] / 2;
            half -= _headers[i].size() / 2;

            stream << std::string(_cell_padding, ' ') << std::setw(_column_sizes[i]) << std::left
                << std::string(half, ' ') + _headers[i] << std::string(_cell_padding, ' ');

            if (_print_style != PrintStyle::SIMPLE && _print_style != PrintStyle::EMPTY)
                stream << "|";
            else
                stream << " ";
        }

        stream << "\n";

        // Print out the line below the header
        if (_print_style != PrintStyle::EMPTY)
            print_plus(stream);

        // Now print the rows of the table
        for (auto& row : _data)
        {
            if (_print_style != PrintStyle::SIMPLE && _print_style != PrintStyle::EMPTY)
                stream << "|";
            else
                stream << " ";

            print_each(row, stream);
            stream << "\n";
            if (_print_style == PrintStyle::FULL)
                print_plus(stream);
        }

        // Print out the line below the header
        if (_print_style == PrintStyle::BASIC)
            print_plus(stream);
    }

    /**
     * Set how to format numbers for each column
     *
     * Note: this is ignored for std::string columns
     *
     * @column_format The format for each column: MUST be the same length as the number of columns.
     */
    void setColumnFormat(const std::vector<VarTableColumnFormat>& column_format)
    {
        assert(column_format.size() == std::tuple_size<DataTuple>::value);

        _column_format = column_format;
    }

    /**
     * Set print style
     */
    void setPrintStyle(const PrintStyle& print_style) { _print_style = print_style; }

    /**
     * Set alignment style
     */
    void setAlignmentStyle(const AlignmentStyle& alignment_style)
    {
        _alignment_style = alignment_style;
    }

    /**
     * Set how many digits of precision to show for floating point numbers
     *
     * Note: this is ignored for std::string columns
     *
     * @column_format The precision for each column: MUST be the same length as the number of columns.
     */
    void setColumnPrecision(const std::vector<int>& precision)
    {
        assert(precision.size() == std::tuple_size<DataTuple>::value);
        _precision = precision;
    }

protected:
    // Just some handy typedefs for the following two functions
    typedef decltype(&std::right) align_type;

    // Attempts to figure out the alignment style
    static align_type justify(AlignmentStyle align)
    {
        if (align == AlignmentStyle::LEFT)
            return std::left;
        else if (align == AlignmentStyle::RIGHT)
            return std::right;
        else
            return std::internal;
    }

    /**
     * These three functions print out each item in a Tuple into the table
     *
     * Original Idea From From https://stackoverflow.com/a/26908596
     *
     * BTW: This would all be a lot easier with generic lambdas
     * there would only need to be one of this sequence and then
     * you could pass in a generic lambda.  Unfortunately, that's C++14
     */

     /**
      *  This ends the recursion
      */
    template <typename TupleType, typename StreamType>
    void print_each(TupleType&&,
        StreamType& /*stream*/,
        std::integral_constant<
        size_t,
        std::tuple_size<typename std::remove_reference<TupleType>::type>::value>)
    {
    }

    /**
     * This gets called on each item
     */
    template <std::size_t I,
        typename TupleType,
        typename StreamType,
        typename = typename std::enable_if<
        I != std::tuple_size<typename std::remove_reference<TupleType>::type>::value>::type>
        void print_each(TupleType&& t, StreamType& stream, std::integral_constant<size_t, I>)
    {
        auto& val = std::get<I>(t);

        // Set the precision
        if (!_precision.empty())
        {
            assert(_precision.size() ==
                std::tuple_size<typename std::remove_reference<TupleType>::type>::value);

            stream << std::setprecision(_precision[I]);
        }

        // Set the format
        if (!_column_format.empty())
        {
            assert(_column_format.size() ==
                std::tuple_size<typename std::remove_reference<TupleType>::type>::value);

            if (_column_format[I] == VarTableColumnFormat::SCIENTIFIC)
                stream << std::scientific;

            if (_column_format[I] == VarTableColumnFormat::FIXED)
                stream << std::fixed;

            if (_column_format[I] == VarTableColumnFormat::PERCENT)
                stream << std::fixed << std::setprecision(2);
        }

        stream << std::string(_cell_padding, ' ') << std::setw(_column_sizes[I])
            << justify(_alignment_style[I]);
        stream << val << std::string(_cell_padding, ' ');

        if (_print_style != PrintStyle::SIMPLE && _print_style != PrintStyle::EMPTY)
            stream << "|";
        else
            stream << " ";

        // Unset the format
        if (!_column_format.empty())
        {
            // Because "stream << std::defaultfloat;" won't compile with old GCC or Clang
            stream.unsetf(std::ios_base::floatfield);
        }

        // Recursive call to print the next item
        print_each(std::forward<TupleType>(t), stream, std::integral_constant<size_t, I + 1>());
    }

    /**
     * his is what gets called first
     */
    template <typename TupleType, typename StreamType>
    void print_each(TupleType&& t, StreamType& stream)
    {
        print_each(std::forward<TupleType>(t), stream, std::integral_constant<size_t, 0>());
    }

    /**
     * Try to find the size the column will take up
     *
     * If the datatype has a size() member... let's call it
     */
    template <class T>
    size_t sizeOfData(const T& data, decltype(((T*)nullptr)->size())* /*dummy*/ = nullptr)
    {
        return data.size();
    }

    /**
     * Try to find the size the column will take up
     *
     * If the datatype is an integer - let's get it's length
     */
    template <class T>
    size_t sizeOfData(const T& data,
        typename std::enable_if<std::is_integral<T>::value>::type* /*dummy*/ = nullptr)
    {
        if (data == 0)
            return 1;

        return std::log10(data) + 1;
    }

    /**
     * If it doesn't... let's just use a statically set size
     */
    size_t sizeOfData(...) { return _static_column_size; }

    /**
     * These three functions iterate over the Tuple, find the printed size of each element and set it
     * in a vector
     */

     /**
      * End the recursion
      */
    template <typename TupleType>
    void size_each(TupleType&&,
        std::vector<unsigned int>& /*sizes*/,
        std::integral_constant<
        size_t,
        std::tuple_size<typename std::remove_reference<TupleType>::type>::value>)
    {
    }

    /**
     * Recursively called for each element
     */
    template <std::size_t I,
        typename TupleType,
        typename = typename std::enable_if<
        I != std::tuple_size<typename std::remove_reference<TupleType>::type>::value>::type>
        void
        size_each(TupleType&& t, std::vector<unsigned int>& sizes, std::integral_constant<size_t, I>)
    {
        sizes[I] = sizeOfData(std::get<I>(t));

        // Override for Percent
        if (!_column_format.empty())
            if (_column_format[I] == VarTableColumnFormat::PERCENT)
                sizes[I] = 6; // 100.00

            // Continue the recursion
        size_each(std::forward<TupleType>(t), sizes, std::integral_constant<size_t, I + 1>());
    }

    /**
     * The function that is actually called that starts the recursion
     */
    template <typename TupleType>
    void size_each(TupleType&& t, std::vector<unsigned int>& sizes)
    {
        size_each(std::forward<TupleType>(t), sizes, std::integral_constant<size_t, 0>());
    }

    /**
     * Print the rows of one, three and last
     */
    template <typename StreamType>
    void print_plus(StreamType& stream)
    {
        switch (_print_style)
        {
        case PrintStyle::BASIC:
        case PrintStyle::FULL:
            stream << "+";
            for (unsigned int i = 0; i < _num_columns; i++)
                stream << std::string(_column_sizes[i] + (2 * _cell_padding), '-') << "+";

            stream << "\n";
            break;
        case PrintStyle::SIMPLE:
            stream << " ";
            for (unsigned int i = 0; i < _num_columns; i++)
                stream << std::string(_column_sizes[i] + (2 * _cell_padding), '-') << " ";

            stream << "\n";
            break;
        default:
            break;
        }
    }
    /**
     * Finds the size each column should be and set it in _column_sizes
     */
    void size_columns()
    {
        _column_sizes.resize(_num_columns);

        // Temporary for querying each row
        std::vector<unsigned int> column_sizes(_num_columns);

        // Start with the size of the headers
        for (unsigned int i = 0; i < _num_columns; i++)
            _column_sizes[i] = _headers[i].size();

        // Grab the size of each entry of each row and see if it's bigger
        for (auto& row : _data)
        {
            size_each(row, column_sizes);

            for (unsigned int i = 0; i < _num_columns; i++)
                _column_sizes[i] = std::max(_column_sizes[i], column_sizes[i]);
        }
    }

    /// The column headers
    std::vector<std::string> _headers;

    /// Number of columns in the table
    unsigned int _num_columns;

    /// Size of columns that we can't get the size of
    unsigned int _static_column_size;

    /// Size of the cell padding
    unsigned int _cell_padding;

    /// The actual data
    std::vector<DataTuple> _data;

    /// Holds the printable width of each column
    std::vector<unsigned int> _column_sizes;

    /// Column Format
    std::vector<VarTableColumnFormat> _column_format;

    /// Print Style
    PrintStyle _print_style;

    /// Alignment Style
    std::vector<AlignmentStyle> _alignment_style;

    /// Precision For each column
    std::vector<int> _precision;
};
