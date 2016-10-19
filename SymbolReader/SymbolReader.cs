﻿using System;
using System.Diagnostics.Contracts;
using System.Text;
using Dia2Lib;

namespace ReClassNET.SymbolReader
{
	public class SymbolReader : IDisposable
	{
		private ComDisposableWrapper<DiaSource> diaSource;
		private ComDisposableWrapper<IDiaSession> diaSession;

		private string searchPath;

		public SymbolReader(string searchPath)
		{
			diaSource = new ComDisposableWrapper<DiaSource>(new DiaSource());

			this.searchPath = searchPath;
		}

		protected virtual void Dispose(bool disposing)
		{
			if (diaSource != null)
			{
				diaSource.Dispose();
				diaSource = null;

				if (diaSession != null)
				{
					diaSession.Dispose();
					diaSession = null;
				}
			}
		}

		~SymbolReader()
		{
			Dispose(false);
		}

		public void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		public static SymbolReader FromModule(RemoteProcess.Module module, string searchPath)
		{
			Contract.Requires(module != null);

			var reader = new SymbolReader(searchPath);
			reader.diaSource.Interface.loadDataForExe(module.Path, searchPath, null);
			reader.CreateSession();
			return reader;
		}

		public static SymbolReader FromDatabase(string path)
		{
			Contract.Requires(!string.IsNullOrEmpty(path));

			var reader = new SymbolReader(null);
			reader.diaSource.Interface.loadDataFromPdb(path);
			reader.CreateSession();
			return reader;
		}

		private void CreateSession()
		{
			IDiaSession session;
			diaSource.Interface.openSession(out session);

			diaSession = new ComDisposableWrapper<IDiaSession>(session);
		}

		public string GetSymbolString(IntPtr address, RemoteProcess.Module module)
		{
			var rva = address.Sub(module.Start);

			IDiaSymbol diaSymbol;
			diaSession.Interface.findSymbolByRVA((uint)rva.ToInt32(), SymTagEnum.SymTagNull, out diaSymbol);
			if (diaSymbol != null)
			{
				using (var symbol = new ComDisposableWrapper<IDiaSymbol>(diaSymbol))
				{
					var sb = new StringBuilder();
					ReadSymbol(diaSymbol, sb);
					return sb.ToString();
				}
			}
			return null;
		}

		private void ReadSymbol(IDiaSymbol symbol, StringBuilder sb)
		{
			/*switch ((SymTagEnum)symbol.symTag)
			{
				case SymTagEnum.SymTagData:
					ReadData(symbol, sb);
					break;
				case SymTagEnum.SymTagFunction:
					sb.Append(symbol.callingConvention.ToString());
					ReadName(symbol, sb);
					break;
				case SymTagEnum.SymTagBlock:
					sb.AppendFormat("len({0:X08}) ", symbol.length);
					ReadName(symbol, sb);
					break;
			}

			ReadSymbolType(symbol, sb);*/
			ReadName(symbol, sb);
		}

		private void ReadSymbolType(IDiaSymbol symbol, StringBuilder sb)
		{
			if (symbol.type != null)
			{
				using (var type = new ComDisposableWrapper<IDiaSymbol>(symbol.type))
				{
					ReadType(type.Interface, sb);
				}
			}
		}

		private void ReadType(IDiaSymbol symbole, StringBuilder sb)
		{
			throw new NotImplementedException();
		}

		private void ReadName(IDiaSymbol symbol, StringBuilder sb)
		{
			if (string.IsNullOrEmpty(symbol.name))
			{
				return;
			}

			if (!string.IsNullOrEmpty(symbol.undecoratedName))
			{
				// If symbol.name equals symbol.undecoratedName there is some extra stuff which can't get undecorated. Try to fix it.
				if (symbol.name == symbol.undecoratedName)
				{
					if (symbol.name.StartsWith("@ILT+"))
					{
						var start = symbol.name.IndexOf('(');
						if (start != -1)
						{
							var name = symbol.name.Substring(start + 1, symbol.name.Length - 1 - start - 1);
							sb.Append(Natives.UnDecorateSymbolName(name));
						}
					}
				}
				else
				{
					sb.Append(symbol.undecoratedName);
				}
			}
			else
			{
				sb.Append(symbol.name);
			}
		}

		private void ReadData(IDiaSymbol symbol, StringBuilder sb)
		{
			throw new NotImplementedException();
		}
	}
}
