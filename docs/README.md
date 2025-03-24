# libmyrtx Documentation

This directory contains the documentation for the libmyrtx library. The documentation is built using Sphinx, with Doxygen integration via Breathe.

## Directory Structure

- `source/`: Source files for the documentation
  - `api/`: API reference documentation
  - `guides/`: Usage guides for different components
  - `examples/`: Example code and explanations
- `build/`: Generated documentation (not tracked in git)
- `doxygen_output/`: Generated Doxygen files (not tracked in git)
- `Makefile`: Makefile for building the documentation
- `Doxyfile`: Configuration for Doxygen
- `requirements.txt`: Python package requirements

## Building the Documentation

### Prerequisites

- Python 3.6 or higher
- Doxygen 1.9.1 or higher

### Setup

Install the required Python packages:

```bash
pip install -r requirements.txt
```

### Building

To build the HTML documentation:

```bash
make html
```

The generated documentation will be available in `build/html/`.

To build with Doxygen integration:

```bash
make prepare html
```

To clean the build directory:

```bash
make clean
```

## Contributing to Documentation

### Adding a New Section

1. Create a new `.rst` file in the appropriate directory (`api/`, `guides/`, or `examples/`)
2. Add the new file to the table of contents in the corresponding `index.rst` file
3. Build the documentation to ensure it renders correctly

### Documentation Guidelines

- Use clear, concise language
- Include code examples when appropriate
- Document all parameters, return values, and exceptions
- Provide usage examples for complex functions
- Link related sections and cross-reference when useful

### reStructuredText Syntax

Basic reStructuredText syntax:

```rst
Section Title
============

Subsection Title
--------------

**Bold text**

*Italic text*

``inline code``

.. code-block:: c

   // Code block
   int main() {
       return 0;
   }

- Bullet point
- Another bullet point

1. Numbered item
2. Another numbered item

See `Link text <https://example.com>`_
```

## ReadTheDocs Integration

This documentation is configured to be built automatically by ReadTheDocs. The configuration file for ReadTheDocs is at the root of the repository (`.readthedocs.yml`).

## License

The documentation is licensed under the same license as the libmyrtx library. 