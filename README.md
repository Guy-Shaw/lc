# lc -- List directory contents by category

`lc` lists the contents of a directory by category.
"Category" roughly means file type, with a few wrinkles.

For each category that has any members,
a category header is printed,
followed by an indented multicolumn listing
of its members.

Categories are:

    * d Directories
    * f Files
    * l Symbolic Links
    * u Unresolved Symbolic Links
    * p Named pipes (FIFO)
    * s Sockets
    * b Block special files
    * c Character special files
    * ? Other
    * E ERROR

Unresolved symbolic links are treated as a separate
category from resolved symbolic links.

Since the first 8 categories are supposed to be exhaustive,
it is unlikely that you will ever see the "Other" category.
It is there, just in case.  If there is ever a file that has
a mode that is not one of the know file types, it is classified
as "Other".

The "ERROR" category is there to cover the case of an error
while reading a directory entry.

Within each category, the multi-column listing is printed
using the same logic as used by `mcml`, which is the same
logic for deciding how to fit columnar data that is used
by GNU `ls`.


## Options

--debug

Pretty-print values of interest only for debugging.

--verbose

Show some feedback while running.


--types=_types_

where _types_ is one or more single letter file types (categories).

The default is to show all file types.


--width=_n_

Use _n_ as the display width.

If no `--width` is specified, then
the display width is set to the environment variable, 'COLUMNS',
if it exists.  If not, then the result of `tput cols` is used,
provided the output is a terminal.
If screen columns cannot be determined, then the default is 80 characters.

`lc` knows nothing of fonts,
and certainly does not provide for variable width fonts.

--show-dirs=_WHEN_

Show the name of each directory.

WHEN is one of: { never always any auto }

'never' and 'always' are self-explanatory.

'any' means show top-level directory names,
only if there are any directories explicitly named
on the command line.

'auto' means show top-level directory names,
only if there are multiple directory names on the command line.


--indent-global=_n_

Indent the entire output by _n_ spaces.

Default: --indent-global=0

--indent-types=_n_

Indent the file type labels (categories) by _n_ spaces.

Default: --indent-types=0


--indent-files=_n_

Indent each data line with _n_ spaces.
No provision is made for tabs.

Default: --indent-files=2

The output has three levels:

1. directory
2. file types (category)
3. file names

Indentation is cumulative.
That is, the number of spaces used to indent a line on any level
is not an absolute number of columns, but a number of columns
to further indent relative to the indentation of its parent.

## History

A program called `lc` that does this kind of listing
has been around since 1984.  Kent Landfield posted
`lc` to `comp.sources.misc` in 1990.  This version
does not try to be an exact work-alike.

Earlier versions of my implementation of `lc`
generated `mcml` markup and piped to `mcml`.

The Kent Landfield `lc` used a simpler, less aggressive
method to determine the widths of each column.
It could sometimes mean the difference between being
forced to list a single column vs being able to fit
the data into two columns, because GNU `lc` logic
uses a more sophisticated algorithm to fit the data.

## License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

##

-- Guy Shaw

   gshaw@acm.org

