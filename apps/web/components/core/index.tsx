import cn from 'clsx'
import { Link } from 'nextra-theme-docs'
import type { FC, MouseEventHandler, ReactNode } from 'react'

export const Button: FC<{
  children: ReactNode
  href?: string
  onClick?: MouseEventHandler
  disabled?: boolean
}> = ({ children, href, onClick, disabled }) => {
  const external = href?.startsWith('http')
  const btnClass = cn(
    'text-white font-medium rounded-lg text-sm px-5 py-2.5 text-center me-2 mb-2 focus:outline-none',
    !disabled
      ? 'bg-gradient-to-r from-teal-400 via-teal-500 to-teal-600 hover:bg-gradient-to-br focus:ring-4 focus:ring-teal-300 dark:focus:ring-teal-800'
      : 'text-gray-900 bg-white border border-gray-200 dark:bg-gray-800 dark:text-gray-400 dark:border-gray-600 cursor-not-allowed'
  )

  return (
    <>
      {href ? (
        <Link
          className={btnClass}
          href={href}
          target={external ? '_blank' : undefined}
          rel={external ? 'noopener noreferrer' : undefined}
          onClick={onClick}
          aria-disabled={disabled}
        >
          {children}
        </Link>
      ) : (
        <button
          type="button"
          className={btnClass}
          onClick={onClick}
          disabled={disabled}
          aria-disabled={disabled}
        >
          {children}
        </button>
      )}
    </>
  )
}

export const Input: FC<{
  id: string
  label: string
  disabled?: boolean
  type?: 'text' | 'password' | 'textarea'
  value?: string
  onChange?: (e: any) => void
  placeholder?: string
  isInvalid?: boolean
  message?: string
  minLength?: number
  maxLength?: number
}> = ({
  id,
  label,
  type = 'text',
  disabled,
  value,
  onChange,
  placeholder,
  isInvalid,
  message,
  minLength,
  maxLength,
}) => {
  const props = {
    id,
    disabled,
    value,
    onChange,
    placeholder,
    minLength,
    maxLength,
    className: cn(
      'block w-full rounded-lg border text-gray-900 dark:text-white placeholder-gray-700 dark:placeholder-gray-100',
      !isInvalid
        ? 'border-teal-500 bg-teal-50 p-2.5 text-sm focus:border-teal-500 focus:ring-teal-500 dark:border-teal-500 dark:bg-gray-700'
        : 'border-red-500 bg-red-50 p-2.5 text-sm focus:border-red-500 focus:ring-red-500 dark:border-red-500 dark:bg-gray-700'
    ),
  }

  return (
    <>
      <label
        htmlFor={id}
        className={cn(
          'mb-2 block text-sm font-medium',
          !isInvalid
            ? 'text-teal-700 dark:text-teal-500'
            : 'text-red-700 dark:text-red-500'
        )}
      >
        {label}
      </label>
      {type !== 'textarea' ? (
        <input type={type} {...props} />
      ) : (
        <textarea rows={4} {...props} />
      )}
      {message && (
        <p
          className={cn(
            'mt-2 text-sm',
            !isInvalid
              ? 'text-teal-700 dark:text-teal-500'
              : 'text-red-700 dark:text-red-500'
          )}
        >
          {message}
        </p>
      )}
    </>
  )
}

export const Select: FC<{
  id: string
  label: string
  children: ReactNode
  onChange?: (e: any) => void
  disabled?: boolean
}> = ({ id, label, children, onChange, disabled }) => {
  return (
    <>
      <label
        htmlFor={id}
        className="mb-2 block text-sm font-medium text-teal-700 dark:text-teal-500"
      >
        {label}
      </label>
      <select
        id={id}
        onChange={onChange}
        disabled={disabled}
        className="block w-full rounded-lg border border-teal-500 bg-gray-50 p-2.5 text-sm text-gray-900 focus:border-teal-500 focus:ring-teal-500 dark:border-teal-500 dark:bg-gray-700 dark:text-white dark:placeholder-gray-400 dark:focus:border-teal-500 dark:focus:ring-teal-500"
      >
        {children}
      </select>
    </>
  )
}
