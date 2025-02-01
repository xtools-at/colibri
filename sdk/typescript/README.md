
# TS NPM Package Boilerplate (2025)

This TypeScript NPM package boilerplate is designed to kickstart the development of TypeScript libraries for Node.js and the browser. It features a modern build setup with TypeScript, leveraging `tsup` for bundling and `@changesets/cli` for version management. The package exports a simple function as an example to demonstrate the setup.

## Features

- TypeScript for type safety.
- Biome for linting and formatting.
- Dual package output (CommonJS and ESM) for compatibility.
- Type definitions for TypeScript projects.
- Automated build and release scripts.

## Prerequisites

- Node.js v22.5.1 (ensure you have this version by using `.nvmrc`)
- `pnpm` (Follow [pnpm installation guide](https://pnpm.io/installation) if you haven't installed it)
- [Biome](https://biomejs.dev/) for linting and formatting

## Reuse

### Step 1: Clone the Boilerplate Repository

First, clone the existing repository `simonorzel26/npm-package-boilerplate-2025` to your local machine. This step involves copying all the files from the original repository.

```bash
git clone https://github.com/simonorzel26/npm-package-boilerplate-2025.git <your-new-repository-name>
cd <your-new-repository-name>
```

### Step 2: Remove the Existing Git History

Since you're creating a new project, you'll likely want to start with a clean history:

```bash
rm -rf .git
```

This command removes the `.git` directory which contains all the git history of the original repository.

### Step 3: Initialize a New Repository

Now, initialize a new git repository:

```bash
git init
git add .
git commit -m "Initial commit based on npm-package-boilerplate-2025"
```

### Step 4: Create a New Repository on GitHub

Go to GitHub and create a new repository named `<your-new-repository-name>`. Do not initialize it with a README, .gitignore, or license since you are importing an existing project.

### Step 5: Push to GitHub

Link your local repository to the GitHub repository and push the changes:

```bash
git remote add origin https://github.com/<your-username>/<your-new-repository-name>.git
git branch -M main
git push -u origin main
```

Replace `<your-username>` with your GitHub username.

## Installation

To use this boilerplate for your project, clone the repository and install the dependencies.

```bash
pnpm install
```

## Usage

After installation, you can start using the boilerplate to build your TypeScript library. Here's how to import and use the example function exported by this package:

```typescript
import { foo } from 'your-package-name';

console.log(foo('Hello, world!'));
```

## Development

This package includes several scripts to help with development:

- `pnpm run build`: Compiles the TypeScript source code and generates both CommonJS and ESM modules along with type definitions.
- `pnpm run lint`: Runs TypeScript compiler checks without emitting code to ensure type safety.
- `pnpm run release`: Bundles the package and publishes it to NPM with version management.

### Adding New Functions

To add a new function, create a `.ts` file in the `src` directory. For example:

```typescript
// src/newFunction.ts
export const newFunction = (): void => {
  // Implementation here
};
```

Then, export it from `index.ts`:

```typescript
// src/index.ts
export * from './newFunction';
```

## Contributing

Contributions are welcome! Please submit a pull request or create an issue for any features, bug fixes, or improvements.

## License

This project is open-sourced under the MIT License. See the [LICENSE](https://github.com/simonorzel26/ts-npm-package-boilerplate-2025/blob/main/LICENSE) file for more details.

## Author

Simon Orzel
