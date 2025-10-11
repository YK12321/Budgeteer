# Changelog

All notable changes to Budgeteer will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial project setup
- C++ backend API with object-oriented design
- Frontend web application
- CI/CD pipelines with GitHub Actions

## [1.0.0] - 2025-10-11

### Added
- **Backend API**
  - Item model class with full encapsulation
  - Database class with CSV parsing
  - API server with multiple query endpoints
  - Levenshtein distance algorithm for fuzzy search
  - Similarity scoring system (0.0 - 1.0)
  - Price statistics calculation (min, max, average)
  - Multi-criteria search (name, description, categories)
  
- **Frontend Application**
  - Modern responsive web interface
  - Smart search with real-time results
  - Store filter (Walmart, Loblaws, Costco)
  - Category filter (10+ categories)
  - Price range filter
  - Multiple sort options (relevance, price, name)
  - Item detail modal with price history
  - Price statistics visualization
  
- **Data**
  - 6,678+ price records
  - 3 store support (Walmart, Loblaws, Costco)
  - 160+ unique products
  - Historical price data (2022-2025)
  - 10+ product categories
  
- **Development**
  - CMake build system
  - Makefile support
  - Build script for Windows
  - Comprehensive documentation
  - .gitignore configuration
  
- **CI/CD**
  - GitHub Actions workflows
  - Multi-platform builds (Windows, Linux, macOS)
  - Automated releases
  - Code quality checks
  - Security scanning
  - GitHub Pages deployment for frontend

### Features
- Search across 6,678+ items
- Compare prices across 3 stores
- Track price history over time
- Filter by store, category, and price
- Sort results by relevance or price
- View detailed price statistics
- Responsive design for all devices

### Technical Details
- C++17 standard
- Object-oriented architecture
- STL containers and algorithms
- Vanilla JavaScript (no frameworks)
- Modern CSS3 with Grid and Flexbox
- Semantic HTML5

### Documentation
- Backend README with API documentation
- Frontend README with setup guide
- Main README with project overview
- Getting Started guide
- Code comments and documentation

## [0.1.0] - 2025-10-11

### Added
- Project initialization
- Basic backend structure
- Basic frontend structure
- Sample dataset

---

## Version Guidelines

- **Major version** (X.0.0): Breaking changes or major new features
- **Minor version** (0.X.0): New features, backward compatible
- **Patch version** (0.0.X): Bug fixes, backward compatible

## Release Process

1. Update CHANGELOG.md with new version details
2. Commit changes: `git commit -am "Release vX.Y.Z"`
3. Create tag: `git tag -a vX.Y.Z -m "Version X.Y.Z"`
4. Push changes: `git push origin main --tags`
5. GitHub Actions will automatically create the release

## Links

- [Repository](https://github.com/YK12321/Budgeteer)
- [Issues](https://github.com/YK12321/Budgeteer/issues)
- [Releases](https://github.com/YK12321/Budgeteer/releases)
