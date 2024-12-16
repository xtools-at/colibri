## Build and publish lib

- sign up and get access token from [npmjs](https://npmjs.com)

### via Github actions

- add `NPM_TOKEN` to _Github actions_ secrets
- create a Github access token and add it to the secrets as `RELEASE_TOKEN`
- bump the package version in `package.json`
- add annotated tag to branch:

```shell
git tag -a v0.2.0 -m "release 0.2.0"
```

- push all tags

```shell
git push origin master --follow-tags
```

### manually

- install Node.js v16+
- install repo dependencies using `npm i` (_not_ yarn)
- add a `.npmrc` file to the project root, with the following contents:

```
//registry.npmjs.org/:_authToken=YourNpmAccessTokenGoesHere
registry=https://registry.npmjs.org/
```

- bump the package version in `package.json`
- build the package

```shell
npm run build
```

- publish to npm

```shell
npm publish --access public
```
