# Contributing to Budgeteer

Thank you for your interest in contributing to Budgeteer! 🎉

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Process](#development-process)
- [Pull Request Process](#pull-request-process)
- [Coding Standards](#coding-standards)
- [Commit Guidelines](#commit-guidelines)

## Code of Conduct

This project follows a Code of Conduct to ensure a welcoming environment:

- Be respectful and inclusive
- Accept constructive criticism
- Focus on what's best for the community
- Show empathy towards others

## Getting Started

1. **Fork the repository**
   ```bash
   git clone https://github.com/YK12321/Budgeteer.git
   cd Budgeteer
   ```

2. **Create a branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Set up development environment**
   - For Backend: Follow [Backend/README.md](Backend/README.md)
   - For Frontend: Open `Frontend/index.html` in browser

## Development Process

### Backend Development

1. Make changes to C++ files in `Backend/src/` or `Backend/include/`
2. Build and test:
   ```bash
   cd Backend
   cmake -B build
   cmake --build build
   ./build/bin/BudgeteerAPI
   ```
3. Ensure code compiles without errors

### Frontend Development

1. Make changes to HTML, CSS, or JavaScript files in `Frontend/`
2. Test in browser by opening `index.html`
3. Ensure responsive design works on mobile/tablet/desktop
4. Validate HTML and CSS

## Pull Request Process

1. **Update Documentation**
   - Update README.md if needed
   - Add comments to complex code
   - Update CHANGELOG.md

2. **Test Your Changes**
   - Backend: Build and run executable
   - Frontend: Test in multiple browsers
   - Ensure no breaking changes

3. **Create Pull Request**
   - Provide clear description of changes
   - Reference any related issues
   - Include screenshots for UI changes

4. **Wait for Review**
   - Address any feedback
   - Make requested changes
   - Be patient and respectful

## Coding Standards

### C++ Guidelines

- Use C++17 standard
- Follow object-oriented principles
- Use meaningful variable names
- Add comments for complex logic
- Keep functions small and focused
- Use const correctness

**Example:**
```cpp
class Database {
private:
    std::vector<Item> items;
    
public:
    std::vector<Item> getItemById(int itemId) const;
};
```

### JavaScript Guidelines

- Use ES6+ features
- Use const/let instead of var
- Use descriptive variable names
- Add JSDoc comments for functions
- Follow functional programming where appropriate

**Example:**
```javascript
/**
 * Performs search and displays results
 * @param {string} query - Search query
 */
function performSearch(query) {
    // Implementation
}
```

### CSS Guidelines

- Use CSS variables for colors
- Mobile-first responsive design
- Use meaningful class names
- Group related styles together

**Example:**
```css
:root {
    --primary: #4F46E5;
}

.search-bar {
    /* Styles */
}
```

## Commit Guidelines

Use clear, descriptive commit messages:

```
feat: Add price history chart to item details
fix: Resolve search bug with special characters
docs: Update API documentation
style: Format code according to style guide
refactor: Simplify search algorithm
test: Add unit tests for Database class
chore: Update dependencies
```

### Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code formatting
- `refactor`: Code restructuring
- `test`: Adding tests
- `chore`: Maintenance tasks

## Issue Guidelines

When creating an issue:

1. **Search existing issues** first
2. **Use issue templates** if available
3. **Provide details**:
   - Clear description
   - Steps to reproduce (for bugs)
   - Expected vs actual behavior
   - Screenshots if applicable
   - Environment details (OS, browser, etc.)

## Feature Requests

We welcome feature requests! Please:

1. Check if feature already exists
2. Describe the feature clearly
3. Explain use case and benefits
4. Consider implementation complexity

## Questions?

- Open an issue with the "question" label
- Check existing documentation
- Review closed issues for similar questions

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

Thank you for contributing to Budgeteer! 🚀
