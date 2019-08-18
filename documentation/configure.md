# API Documentation for Configure

- [API Documentation for Configure](#api-documentation-for-configure)
	- [NoPoDoFo Configure](#nopodofo-configure)
	- [Constructors](#constructors)
	- [Properties](#properties)
	    - [logLevel](#loglevel)
	- [Methods](#methods)
		- [logFile](#logfile)
		- [logOnInterval](#logoninterval)
	
## NoPoDoFo Configure
Configure exposes runtime configuration options and loggers.

## Constructors

## Properties

### logLevel

Set the log level as one of NPDFLogLevel
## Methods

### logFile

```typescript
logFile(output: string): void
```

Initializes the logger and sets the logger output location.

### logOnInterval

```typescript
logOnInterval(n: number): void
```

Set's the logger to flush every [n] seconds
